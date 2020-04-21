/**
 * Name: Snowy Chen, Joe Song
 * Date: 8 April 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::Cpp

#include "distributed_value.h"
#include "dataframe.h"
#include "key_value_store.h"

DistributedValue::DistributedValue(Key &key, Schema &schema,
                                   KeyValueStore *kv) {
  assert(kv);

  this->key = new Key(key.get_name()->c_str(), key.get_home_id());
  this->schema = new Schema(schema);

  this->kv = kv;

  this->num_rows = 0;

  this->cached_key = nullptr;
  this->cached_df = nullptr;

  this->writer_node_id = 0;
  this->writer_curr_num_segments = 0;
  this->writer_curr_row_num = 0;
  this->building_df = nullptr;
}

DistributedValue::~DistributedValue() {
  delete this->key;
  delete this->schema;
  delete this->cached_df;
  delete this->cached_key;
}

Schema &DistributedValue::get_schema() { return *this->schema; }

void DistributedValue::cache_df(size_t row) {
  // Calculate the key containing the specified row.
  size_t key_suffix = row / DistributedValue::MAX_NUM_ROWS;
  size_t node_id = key_suffix % this->kv->get_num_nodes();
  String *key_name =
      StrBuff().c(this->key->get_name()->c_str()).c("-").c(key_suffix).get();
  Key *target_key = new Key(key_name->c_str(), node_id);
  delete key_name;

  // Only switch out the cached value if it's not already cached
  if ((this->cached_key == nullptr) ||
      (!target_key->equals(this->cached_key))) {
    if (this->cached_key != nullptr) {
      // Clear out the cache real quick
      delete this->cached_df;
      delete this->cached_key;
    }

    this->cached_key = target_key;
    this->cached_df = this->kv->wait_and_get_df(*this->cached_key);
  } else {
    // No need for this since we already have the cached df
    delete target_key;
  }
}

int DistributedValue::get_int(size_t col, size_t row) {
  this->cache_df(row);
  return this->cached_df->get_int(col, row % DistributedValue::MAX_NUM_ROWS);
}

bool DistributedValue::get_bool(size_t col, size_t row) {
  this->cache_df(row);
  return this->cached_df->get_bool(col, row % DistributedValue::MAX_NUM_ROWS);
}

float DistributedValue::get_float(size_t col, size_t row) {
  this->cache_df(row);
  return this->cached_df->get_float(col, row % DistributedValue::MAX_NUM_ROWS);
}

String *DistributedValue::get_string(size_t col, size_t row) {
  this->cache_df(row);
  return this->cached_df->get_string(col, row % DistributedValue::MAX_NUM_ROWS);
}

size_t DistributedValue::nrows() { return this->num_rows; }

size_t DistributedValue::ncols() { return this->schema->width(); }

void DistributedValue::local_map(Rower &r) {
  this->kv->local_map(this->key->get_name(), r);
}

void DistributedValue::map(Rower &r) {
  size_t num_rows_read = 0;
  while (num_rows_read < this->num_rows) {
    // Cache the next dataframe
    this->cache_df(num_rows_read);
    this->cached_df->map(r);

    // Now increment the rows read by the number of rows in the cached df
    num_rows_read += this->cached_df->nrows();
  }
}

void DistributedValue::serialize(Serializer &serializer) {
  serializer.set_size_t(this->ncols());
  serializer.set_size_t(this->nrows());

  for (size_t c = 0; c < this->ncols(); c++) {
    char data_type = this->get_schema().col_type(c);
    serializer.set_generic(reinterpret_cast<unsigned char *>(&data_type),
                           sizeof(data_type));
  }
}

size_t DistributedValue::serialization_required_bytes() {
  size_t ret_value = Serializer::get_required_bytes(this->ncols());
  ret_value += Serializer::get_required_bytes(this->nrows());
  ret_value += this->ncols();
  return ret_value;
}

DistributedValue *
DistributedValue::deserialize_as_distributed_val(Deserializer &deserializer,
                                                 Key &key, KeyValueStore *kv) {
  assert(key.get_home_id() == -1);
  assert(kv);

  size_t ncols = deserializer.get_size_t();
  size_t nrows = deserializer.get_size_t();

  // Create the schema
  Schema schema;
  for (size_t c = 0; c < ncols; c++) {
    char data_type = (char)deserializer.get_byte();
    schema.add_column(data_type);
  }

  auto *ret_value = new DistributedValue(key, schema, kv);
  ret_value->num_rows = nrows;
  return ret_value;
}

void DistributedValue::add_row(Row &row) {
  if (this->building_df == nullptr) {
    this->building_df = new DataFrame(*this->schema);
  }

  // Add this row
  this->building_df->add_row(row);
  this->num_rows += 1;

  // Increment the row counter
  this->writer_curr_row_num += 1;

  // Store the dataframe if we've reached the maximum number of rows
  if (this->writer_curr_row_num >= DistributedValue::MAX_NUM_ROWS) {
    // Generate the key
    String *key_name = StrBuff()
                           .c(key->get_name()->c_str())
                           .c("-")
                           .c(this->writer_curr_num_segments)
                           .get();
    Key new_key(key_name->c_str(), this->writer_node_id);
    delete key_name;

    // Add the dataframe
    this->kv->put_df(new_key, this->building_df);
    // Delete the created dataframe if we had to send it to a different node
    if (new_key.get_home_id() != this->kv->get_home_id()) {
      delete this->building_df;
    }
    this->writer_curr_num_segments += 1;

    // Now reset everything and prepare for the next dataframe segment
    this->writer_node_id += 1;
    if (this->writer_node_id >= this->kv->get_num_nodes()) {
      this->writer_node_id = 0;
    }
    this->writer_curr_row_num = 0;
    this->building_df = nullptr;
  }
}

void DistributedValue::package_value() {
  // Clean up the last dataframes
  // Add the last dataframe if applicable
  if (this->building_df != nullptr) {
    // Generate the key
    String *key_name = StrBuff()
                           .c(key->get_name()->c_str())
                           .c("-")
                           .c(this->writer_curr_num_segments)
                           .get();
    Key new_key(key_name->c_str(), this->writer_node_id);
    delete key_name;

    // Add the dataframe
    this->kv->put_df(new_key, this->building_df);
    // Delete the created dataframe if we had to send it to a different node
    if (new_key.get_home_id() != this->kv->get_home_id()) {
      delete this->building_df;
    }

    this->writer_node_id += 1;
    if (this->writer_node_id >= this->kv->get_num_nodes()) {
      this->writer_node_id = 0;
    }
    this->writer_curr_row_num = 0;
    this->building_df = nullptr;
  }

  this->kv->put(*this->key, this);
  this->kv->broadcast_value(*this->key, this);
}

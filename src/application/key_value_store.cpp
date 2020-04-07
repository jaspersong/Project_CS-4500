/**
 * Name: Snowy Chen, Joe Song
 * Date: 21 March 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::Cpp

#include "key_value_store.h"
#include "copy_writer.h"
#include "local_network_msg_manager.h"
#include "networked_msg_manager.h"
#include "sorer_integrator.h"
#include <unistd.h>

KeyValueStore::KeyValueStore(size_t num_nodes) {
  assert(num_nodes > 0);

  this->num_nodes = num_nodes;
  if (this->num_nodes == 1) {
    // This is the only node that this system is expecting. Automatically
    // give it the only available node id.
    this->home_node = 0;
  } else {
    // This is one of many nodes. Default to -1, for the registrar, first for
    // now.
    this->home_node = -1;
  }

  this->local_network_layer = nullptr;
  this->real_network_layer = nullptr;
}

KeyValueStore::~KeyValueStore() {
  // Delete all of the keys and dataframes within the map
  for (auto const& x : this->kv_map)
  {
    delete x.first;
    delete x.second;
  }
}

void KeyValueStore::put(Key &key, DataFrame *value) {
  assert((key.get_home_id() == -1) || (key.get_home_id() < this->num_nodes));
  assert(value != nullptr);
  assert(this->verify_distributed_layer());

  if (key.get_home_id() == -1) {
    // Copy the dataframe into different segments
    CopyWriter writer(value);
    size_t node_id = 0;
    size_t num_segments = 0;
    size_t curr_row_num = 0;
    DataFrame *df = nullptr;
    size_t total_rows = 0;
    while (!writer.done()) {
      if (df == nullptr) {
        df = new DataFrame(value->get_schema());
      }

      // Create the row from the writer
      total_rows += 1;
      Row r(value->get_schema());
      writer.visit(r);
      df->add_row(r);

      // Increment the row counter
      curr_row_num += 1;

      // Store the dataframe if we've reached the maximum number of rows
      if (curr_row_num >= KeyValueStore::MAX_NUM_DISTRIBUTED_ROWS) {
        // Generate the key
        String *key_name = StrBuff().c(key.get_name()->c_str()).c("-").
            c(num_segments).get();
        Key new_key(key_name->c_str(), node_id);
        delete key_name;

        // Add the dataframe
        this->put(new_key, df);
        // Delete the created dataframe if we had to send it to a different node
        if (new_key.get_home_id() != this->get_home_id()) {
          delete df;
        }
        num_segments += 1;

        // Now reset everything and prepare for the next dataframe segment
        node_id += 1;
        if (node_id >= this->get_num_nodes()) {
          node_id = 0;
        }
        curr_row_num = 0;
        df = nullptr;
      }
    }

    // Add the last dataframe if applicable
    if (df != nullptr) {
      // Generate the key
      String *key_name = StrBuff().c(key.get_name()->c_str()).c("-").
          c(num_segments).get();
      Key new_key(key_name->c_str(), node_id);
      delete key_name;

      // Add the dataframe
      this->put(new_key, df);
      // Delete the created dataframe if we had to send it to a different node
      if (new_key.get_home_id() != this->get_home_id()) {
        delete df;
      }
    }
  } else if (key.get_home_id() == this->home_node) {
    Key *owned_key = new Key(key.get_name()->c_str(), key.get_home_id());

    this->kv_lock.lock();
    this->kv_map[owned_key] = value;
    this->kv_lock.unlock();
  } else if (this->local_network_layer != nullptr) {
    this->local_network_layer->send_put(key.get_home_id(), key, value);
  } else if (this->real_network_layer != nullptr) {
    this->real_network_layer->send_put(key.get_home_id(), key, value);
  }
}

DataFrame *KeyValueStore::wait_and_get(Key &key) {
  assert((key.get_home_id() == -1) || (key.get_home_id() < this->num_nodes));
  assert(this->verify_distributed_layer());

  if (key.get_home_id() == this->home_node) {
    // Wait until the key is available on this key-value store
    std::map<Key *, DataFrame *, KeyComp>::iterator it;
    it = this->kv_map.find(&key);
    while (it == this->kv_map.end()) {
      sleep(1);
      it = this->kv_map.find(&key);
    }

    this->kv_lock.lock();

    auto *value = this->kv_map[&key];

    // Now copy the value so it can be owned by the caller.
    auto *ret_value = new DataFrame(value->get_schema());
    CopyWriter copier(value);
    while (!copier.done()) {
      Row r(value->get_schema());
      copier.visit(r);
      ret_value->add_row(r);
    }

    this->kv_lock.unlock();

    return ret_value;
  } else if (key.get_home_id() == -1) {
    // TODO??
    assert(false);
  } else if (this->local_network_layer != nullptr) {
    this->local_network_layer->send_waitandget(key.get_home_id(), key);

    // Now wait for the response
    return this->local_network_layer->get_requested_dataframe();
  } else if (this->real_network_layer != nullptr) {
    this->real_network_layer->send_waitandget(key.get_home_id(), key);

    // Now wait for the response
    return this->real_network_layer->get_requested_dataframe();
  } else {
    // We should not be here
    assert(false);
  }
}

DataFrame *KeyValueStore::get_local(Key &key) {
  assert(this->verify_distributed_layer());

  if (key.get_home_id() == this->home_node) {
    // Wait until the key is available on this key-value store
    std::map<Key *, DataFrame *, KeyComp>::iterator it;
    it = this->kv_map.find(&key);
    while (it == this->kv_map.end()) {
      sleep(1);
      it = this->kv_map.find(&key);
    }

    return this->kv_map[&key];
  } else {
    return nullptr;
  }
}

void KeyValueStore::local_map(Key &key, Rower &rower) {
  if (key.get_home_id() == this->home_node) {
    // This dataframe is exclusive on this node
    DataFrame *df = this->get_local(key);
    df->map(rower);
  } else if (key.get_home_id() == -1) {
    // This dataframe is distributed. As a result, the dataframes segments
    // that match this key contains the same first couple of letters.
    String *prefix = key.get_name();

    for (this->start_iter(); this->has_next(); this->next_iter()) {
      String other_prefix(this->get_iter_key()->get_name()->c_str(), prefix->size());
      if (other_prefix.equals(prefix)) {
        // this prefix matches. Map through it with the rower
        DataFrame *df = this->get_iter_value();
        df->map(rower);
      }
    }
  }
}

LocalNetworkMessageManager *KeyValueStore::connect_local(size_t node_id,
                                                         StatusHandler* status_handler) {
  assert(this->local_network_layer == nullptr);
  assert(this->real_network_layer == nullptr);
  assert(this->num_nodes > 1);
  assert(this->home_node == -1);

  this->home_node = node_id;

  this->local_network_layer = new LocalNetworkMessageManager(this, status_handler);
  return this->local_network_layer;
}

void KeyValueStore::register_local(LocalNetworkMessageManager *msg_manager) {
  assert(this->local_network_layer != nullptr);
  this->local_network_layer->register_local(msg_manager);
}

RealNetworkMessageManager *KeyValueStore::connect_network(
    StatusHandler* status_handler) {
  assert(this->local_network_layer == nullptr);
  assert(this->real_network_layer == nullptr);
  assert(this->num_nodes > 1);
  assert(this->home_node == -1);

  this->real_network_layer = new RealNetworkMessageManager(this, status_handler);
  return this->real_network_layer;
}

size_t KeyValueStore::from_array(Key &key, KeyValueStore *kv,
                                     size_t num_values, float *values) {
  assert(kv && values);
  assert((key.get_home_id() == -1) || (key.get_home_id() < kv->num_nodes));

  // Create the dataframe
  Schema schema("F");
  auto *ret_value = new DataFrame(schema);

  for (size_t i = 0; i < num_values; i++) {
    Row r(ret_value->get_schema());
    r.set(0, values[i]);
    ret_value->add_row(r);
  }

  kv->put(key, ret_value);
  // Delete the created dataframe if we had to send it to a different node
  if (key.get_home_id() != kv->get_home_id()) {
    delete ret_value;
  }

  return num_values;
}

size_t KeyValueStore::from_array(Key &key, KeyValueStore *kv,
                                     size_t num_values, int *values) {
  assert(kv && values);
  assert((key.get_home_id() == -1) || (key.get_home_id() < kv->num_nodes));

  // Create the dataframe
  Schema schema("I");
  auto *ret_value = new DataFrame(schema);

  for (size_t i = 0; i < num_values; i++) {
    Row r(ret_value->get_schema());
    r.set(0, values[i]);
    ret_value->add_row(r);
  }

  kv->put(key, ret_value);
  // Delete the created dataframe if we had to send it to a different node
  if (key.get_home_id() != kv->get_home_id()) {
    delete ret_value;
  }

  return num_values;
}

size_t KeyValueStore::from_array(Key &key, KeyValueStore *kv,
                                     size_t num_values, bool *values) {
  assert(kv && values);
  assert((key.get_home_id() == -1) || (key.get_home_id() < kv->num_nodes));

  // Create the dataframe
  Schema schema("B");
  auto *ret_value = new DataFrame(schema);

  for (size_t i = 0; i < num_values; i++) {
    Row r(ret_value->get_schema());
    r.set(0, values[i]);
    ret_value->add_row(r);
  }

  kv->put(key, ret_value);
  // Delete the created dataframe if we had to send it to a different node
  if (key.get_home_id() != kv->get_home_id()) {
    delete ret_value;
  }

  return num_values;
}

size_t KeyValueStore::from_array(Key &key, KeyValueStore *kv,
                                     size_t num_values, String **values) {
  assert(kv && values);
  assert((key.get_home_id() == -1) || (key.get_home_id() < kv->num_nodes));

  // Create the dataframe
  Schema schema("S");
  auto *ret_value = new DataFrame(schema);

  for (size_t i = 0; i < num_values; i++) {
    Row r(ret_value->get_schema());
    r.set(0, values[i]);
    ret_value->add_row(r);
  }

  kv->put(key, ret_value);
  // Delete the created dataframe if we had to send it to a different node
  if (key.get_home_id() != kv->get_home_id()) {
    delete ret_value;
  }

  return num_values;
}

size_t KeyValueStore::from_scalar(Key &key, KeyValueStore *kv, bool value) {
  assert(kv != nullptr);
  assert((key.get_home_id() == -1) || (key.get_home_id() < kv->num_nodes));

  // Create the dataframe
  Schema schema("B");
  auto *ret_value = new DataFrame(schema);

  // Store the value
  Row r(ret_value->get_schema());
  r.set(0, value);
  ret_value->add_row(r);

  kv->put(key, ret_value);
  // Delete the created dataframe if we had to send it to a different node
  if (key.get_home_id() != kv->get_home_id()) {
    delete ret_value;
  }

  return 1;
}

size_t KeyValueStore::from_scalar(Key &key, KeyValueStore *kv, int value) {
  assert(kv != nullptr);
  assert((key.get_home_id() == -1) || (key.get_home_id() < kv->num_nodes));

  // Create the dataframe
  Schema schema("I");
  auto *ret_value = new DataFrame(schema);

  // Store the value
  Row r(ret_value->get_schema());
  r.set(0, value);
  ret_value->add_row(r);

  kv->put(key, ret_value);
  // Delete the created dataframe if we had to send it to a different node
  if (key.get_home_id() != kv->get_home_id()) {
    delete ret_value;
  }

  return 1;
}

size_t KeyValueStore::from_scalar(Key &key, KeyValueStore *kv,
                                      float value) {
  assert(kv != nullptr);
  assert((key.get_home_id() == -1) || (key.get_home_id() < kv->num_nodes));

  // Create the dataframe
  Schema schema("F");
  auto *ret_value = new DataFrame(schema);

  // Store the value
  Row r(ret_value->get_schema());
  r.set(0, value);
  ret_value->add_row(r);

  kv->put(key, ret_value);
  // Delete the created dataframe if we had to send it to a different node
  if (key.get_home_id() != kv->get_home_id()) {
    delete ret_value;
  }

  return 1;
}

size_t KeyValueStore::from_scalar(Key &key, KeyValueStore *kv,
                                      String *value) {
  assert(kv != nullptr);
  assert((key.get_home_id() == -1) || (key.get_home_id() < kv->num_nodes));

  // Create the dataframe
  Schema schema("S");
  auto *ret_value = new DataFrame(schema);

  // Store the value
  Row r(ret_value->get_schema());
  r.set(0, value);
  ret_value->add_row(r);

  kv->put(key, ret_value);
  // Delete the created dataframe if we had to send it to a different node
  if (key.get_home_id() != kv->get_home_id()) {
    delete ret_value;
  }

  return 1;
}

bool KeyValueStore::verify_distributed_layer() {
  // Take the home id from the network layer if this application has been
  // configured to dynamically be enumerated.
  if (this->real_network_layer != nullptr) {
    this->home_node = this->real_network_layer->get_home_id();
  }

  return (this->home_node != -1) && (this->home_node < this->num_nodes) &&
         ((this->num_nodes == 1) || ((this->local_network_layer != nullptr) &&
         this->local_network_layer->verify_configuration()) ||
         ((this->real_network_layer != nullptr) &&
         (this->real_network_layer->get_home_id() != -1)));
}

size_t KeyValueStore::get_home_id() { return this->home_node; }

size_t KeyValueStore::from_visitor(Key &key, KeyValueStore *kv,
                                 const char *schema_types, Writer &writer) {
  assert(kv && schema_types);
  assert((key.get_home_id() == -1) || (key.get_home_id() < kv->num_nodes));

  Schema schema(schema_types);
  auto *df = new DataFrame(schema);

  // Iterate through using the writer
  while (!writer.done()) {
    Row r(schema);
    writer.visit(r);
    df->add_row(r);
  }

  size_t ret_value = df->nrows();

  // Add the df to the kvstore now at the specified key
  kv->put(key, df);
  // Delete the dataframe if it was put in a different kvstore
  if (key.get_home_id() != kv->get_home_id()) {
    delete df;
  }

  return ret_value;
}

size_t KeyValueStore::from_file(Key &key, KeyValueStore *kv,
                              const char *file_name) {
  SorerIntegrator integrator(file_name);
  integrator.parse();
  DataFrame *df = integrator.convert();

  size_t ret_value = df->nrows();

  kv->put(key, df);
  // Delete the dataframe if it was put in a different kvstore
  if (key.get_home_id() != kv->get_home_id()) {
    delete df;
  }

  return ret_value;
}

void KeyValueStore::send_status_message(size_t node_id, String &msg) {
  if (this->local_network_layer != nullptr) {
    this->local_network_layer->send_status(node_id, msg);
  } else if (this->real_network_layer != nullptr) {
    this->real_network_layer->send_status(node_id, msg);
  }
}

void KeyValueStore::start_iter() {
  this->iter = this->kv_map.begin();
}

bool KeyValueStore::has_next() {
  return this->iter != this->kv_map.end();
}

void KeyValueStore::next_iter() {
  this->iter++;
}

Key *KeyValueStore::get_iter_key() {
  return this->iter->first;
}
DataFrame *KeyValueStore::get_iter_value() {
  return this->iter->second;
}

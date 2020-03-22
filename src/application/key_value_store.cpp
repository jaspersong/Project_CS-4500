/**
 * Name: Snowy Chen, Joe Song
 * Date: 21 March 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::Cpp

#include "key_value_store.h"

KeyValueStore::KeyValueStore(size_t home_node) { this->home_node = home_node; }

void KeyValueStore::put(Key &key, DataFrame *value) {
  assert(value != nullptr);

  if (key.get_home_id() == this->home_node) {
    this->kv.put(&key, value);
  } else {
    // TODO: Link the key value stores together so they can talk to each
    // other.
  }
}

DataFrame *KeyValueStore::wait_and_get(Key &key) {
  if (key.get_home_id() == this->home_node) {
    auto *value = reinterpret_cast<DataFrame *>(this->kv.get(&key));

    // Now copy the value so it can be owned by the caller.
    auto *ret_value = new DataFrame(*value);
    // TODO: Create a rower/fielder to copy the dataframe into a new
    //  dataframe.

    return value;
  } else {
    // TODO: Link the key value stores together so they can talk to each
    //  other.
    return nullptr;
  }
}

DataFrame *KeyValueStore::from_array(Key &key, KeyValueStore *kv,
                                     size_t num_values, float *values) {
  assert(kv != nullptr);
  assert(values != nullptr);

  // Create the dataframe
  Schema schema("F");
  auto *ret_value = new DataFrame(schema);

  for (size_t i = 0; i < num_values; i++) {
    Row r(ret_value->get_schema());
    r.set(0, values[i]);
    ret_value->add_row(r);
  }

  kv->put(key, ret_value);

  return ret_value;
}

DataFrame *KeyValueStore::from_array(Key &key, KeyValueStore *kv,
                                     size_t num_values, int *values) {
  assert(kv != nullptr);
  assert(values != nullptr);

  // Create the dataframe
  Schema schema("I");
  auto *ret_value = new DataFrame(schema);

  for (size_t i = 0; i < num_values; i++) {
    Row r(ret_value->get_schema());
    r.set(0, values[i]);
    ret_value->add_row(r);
  }

  kv->put(key, ret_value);

  return ret_value;
}

DataFrame *KeyValueStore::from_array(Key &key, KeyValueStore *kv,
                                     size_t num_values, bool *values) {
  assert(kv != nullptr);
  assert(values != nullptr);

  // Create the dataframe
  Schema schema("B");
  auto *ret_value = new DataFrame(schema);

  for (size_t i = 0; i < num_values; i++) {
    Row r(ret_value->get_schema());
    r.set(0, values[i]);
    ret_value->add_row(r);
  }

  kv->put(key, ret_value);

  return ret_value;
}

DataFrame *KeyValueStore::from_array(Key &key, KeyValueStore *kv,
                                     size_t num_values, String **values) {
  assert(kv != nullptr);
  assert(values != nullptr);

  // Create the dataframe
  Schema schema("S");
  auto *ret_value = new DataFrame(schema);

  for (size_t i = 0; i < num_values; i++) {
    Row r(ret_value->get_schema());
    r.set(0, values[i]);
    ret_value->add_row(r);
  }

  kv->put(key, ret_value);

  return ret_value;
}

DataFrame *KeyValueStore::from_scalar(Key &key, KeyValueStore *kv, bool value) {
  assert(kv != nullptr);

  // Create the dataframe
  Schema schema("B");
  auto *ret_value = new DataFrame(schema);

  // Store the value
  Row r(ret_value->get_schema());
  r.set(0, value);
  ret_value->add_row(r);

  kv->put(key, ret_value);

  return ret_value;
}

DataFrame *KeyValueStore::from_scalar(Key &key, KeyValueStore *kv, int value) {
  assert(kv != nullptr);

  // Create the dataframe
  Schema schema("I");
  auto *ret_value = new DataFrame(schema);

  // Store the value
  Row r(ret_value->get_schema());
  r.set(0, value);
  ret_value->add_row(r);

  kv->put(key, ret_value);

  return ret_value;
}

DataFrame *KeyValueStore::from_scalar(Key &key, KeyValueStore *kv,
                                      float value) {
  assert(kv != nullptr);

  // Create the dataframe
  Schema schema("F");
  auto *ret_value = new DataFrame(schema);

  // Store the value
  Row r(ret_value->get_schema());
  r.set(0, value);
  ret_value->add_row(r);

  kv->put(key, ret_value);

  return ret_value;
}

DataFrame *KeyValueStore::from_scalar(Key &key, KeyValueStore *kv,
                                      String *value) {
  assert(kv != nullptr);

  // Create the dataframe
  Schema schema("S");
  auto *ret_value = new DataFrame(schema);

  // Store the value
  Row r(ret_value->get_schema());
  r.set(0, value);
  ret_value->add_row(r);

  kv->put(key, ret_value);

  return ret_value;
}

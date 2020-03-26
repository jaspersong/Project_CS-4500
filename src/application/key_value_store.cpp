/**
 * Name: Snowy Chen, Joe Song
 * Date: 21 March 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::Cpp

#include "key_value_store.h"
#include "copy_rower.h"
#include "local_network_msg_manager.h"
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

  this->using_local_network = false;
  this->local_network_layer = nullptr;
}

void KeyValueStore::put(Key &key, DataFrame *value) {
  assert(key.get_home_id() < this->num_nodes);
  assert(value != nullptr);
  assert(this->verify_distributed_layer());

  if (key.get_home_id() == this->home_node) {
    this->kv_lock.lock();
    this->kv_map.put(&key, value);
    this->kv_lock.unlock();
  } else if (this->using_local_network) {
    this->local_network_layer->send_put(key.get_home_id(), key, value);
  } else {
    // TODO: Connect with the network layer
    assert(false);
  }
}

DataFrame *KeyValueStore::wait_and_get(Key &key) {
  assert(key.get_home_id() < this->num_nodes);
  assert(this->verify_distributed_layer());

  if (key.get_home_id() == this->home_node) {
    // Wait until the key is available on this key-value store
    while (!this->kv_map.contains_key(&key)) {
      sleep(1);
    }

    this->kv_lock.lock();

    auto *value = reinterpret_cast<DataFrame *>(this->kv_map.get(&key));

    // Now copy the value so it can be owned by the caller.
    auto *ret_value = new DataFrame(*value);
    CopyRower copier(ret_value);
    value->map(copier);

    this->kv_lock.unlock();

    return ret_value;
  } else if (this->using_local_network) {
    this->local_network_layer->send_waitandget(key.get_home_id(), key);

    // Now wait for the response
    return this->local_network_layer->get_requested_dataframe();
  } else {
    // TODO: Connect with the network layer
    assert(false);
    return nullptr;
  }
}

DataFrame *KeyValueStore::get_local(Key &key) {
  assert(this->verify_distributed_layer());

  if (key.get_home_id() == this->home_node) {
    // Wait until the key is available on this key-value store
    while (!this->kv_map.contains_key(&key)) {
      sleep(1);
    }

    return reinterpret_cast<DataFrame *>(this->kv_map.get(&key));
  } else {
    return nullptr;
  }
}

LocalNetworkMessageManager *KeyValueStore::connect_local(size_t node_id) {
  assert(!this->using_local_network);
  assert(this->local_network_layer == nullptr);
  assert(this->num_nodes > 1);
  assert(this->home_node == -1);

  this->home_node = node_id;

  this->using_local_network = true;
  this->local_network_layer = new LocalNetworkMessageManager(this);
  return this->local_network_layer;
}

void KeyValueStore::register_local(LocalNetworkMessageManager *msg_manager) {
  assert(this->using_local_network);
  assert(this->local_network_layer != nullptr);

  this->local_network_layer->register_local(msg_manager);
}

DataFrame *KeyValueStore::from_array(Key &key, KeyValueStore *kv,
                                     size_t num_values, float *values) {
  assert(kv != nullptr);
  assert(values != nullptr);
  assert(key.get_home_id() < kv->num_nodes);

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
  assert(key.get_home_id() < kv->num_nodes);

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
  assert(key.get_home_id() < kv->num_nodes);

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
  assert(key.get_home_id() < kv->num_nodes);

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
  assert(key.get_home_id() < kv->num_nodes);

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
  assert(key.get_home_id() < kv->num_nodes);

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
  assert(key.get_home_id() < kv->num_nodes);

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
  assert(key.get_home_id() < kv->num_nodes);

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

bool KeyValueStore::verify_distributed_layer() {
  return (this->home_node != -1) && (this->home_node < this->num_nodes) &&
         ((this->num_nodes == 1) || (this->using_local_network &&
         this->local_network_layer->verify_configuration()));
}

size_t KeyValueStore::get_home_id() { return this->home_node; }

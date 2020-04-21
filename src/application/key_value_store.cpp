/**
 * Name: Snowy Chen, Joe Song
 * Date: 21 March 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::Cpp

#include "key_value_store.h"
#include "copy_writer.h"
#include "distributed_value.h"
#include "local_network_msg_manager.h"
#include "socket_network_msg_manager.h"
#include "sorer_integrator.h"
#include "thread.h"

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
  for (auto const &x : this->kv_map) {
    delete x.first;
    delete x.second;
  }
  for (auto const &x : this->distro_kv_map) {
    delete x.first;
    delete x.second;
  }
}

DistributedValue *KeyValueStore::wait_and_get(Key &key) {
  assert(this->verify_distributed_layer());
  assert(key.get_home_id() == -1);

  // Wait until the key is available on this key-value store
  this->wait_for_listener(key);

  return this->kv_map[&key];
}

void KeyValueStore::put(Key &key, DistributedValue *value) {
  assert(key.get_home_id() == -1);
  assert(value != nullptr);
  assert(this->verify_distributed_layer());

  // Add this value to the current kv-store
  Key *owned_key = new Key(key.get_name()->c_str(), key.get_home_id());
  this->kv_lock.lock();

  // Delete any values that is currently associated with this key
  std::map<Key *, DistributedValue *, KeyComp>::iterator it;
  it = this->kv_map.find(owned_key);
  if (it != this->kv_map.end()) {
    delete it->first;
    delete it->second;
  }
  // Now add the new value
  this->kv_map[owned_key] = value;
  this->kv_lock.unlock();

  // Signal that we have the value now
  this->signal_listener(key);
}

void KeyValueStore::broadcast_value(Key &key, DistributedValue *value) {
  if (this->local_network_layer != nullptr) {
    this->local_network_layer->broadcast_value(key, value);
  } else if (this->real_network_layer != nullptr) {
    this->real_network_layer->broadcast_value(key, value);
  }
}

void KeyValueStore::put_df(Key &key, DataFrame *value) {
  assert(key.get_home_id() < this->num_nodes);
  assert(value != nullptr);
  assert(this->verify_distributed_layer());

  if (key.get_home_id() == this->home_node) {
    Key *owned_key = new Key(key.get_name()->c_str(), key.get_home_id());

    this->distro_kv_lock.lock();
    std::map<Key *, DataFrame *, KeyComp>::iterator it;
    it = this->distro_kv_map.find(owned_key);
    if (it != this->distro_kv_map.end()) {
      // Remove the key-value pairs that already exist, if applicable
      delete it->first;
      delete it->second;
    }
    this->distro_kv_map[owned_key] = value;
    this->distro_kv_lock.unlock();
  } else if (this->local_network_layer != nullptr) {
    this->local_network_layer->send_put(key.get_home_id(), key, value);
  } else if (this->real_network_layer != nullptr) {
    this->real_network_layer->send_put(key.get_home_id(), key, value);
  } else {
    // Should not get to here
    assert(false);
  }
}

DataFrame *KeyValueStore::wait_and_get_df(Key &key) {
  assert(key.get_home_id() < this->num_nodes);
  assert(this->verify_distributed_layer());

  if (key.get_home_id() == this->home_node) {
    this->distro_kv_lock.lock();

    // Given that all values are distributed, and distributed values are
    // available sent after the distribution of dfs, then we should always
    // have the dataframe associated with the key available
    auto it = this->distro_kv_map.find(&key);
    assert(it != this->distro_kv_map.end());

    auto *value = this->distro_kv_map[&key];
    // Now copy the value so it can be owned by the caller.
    auto *ret_value = new DataFrame(value->get_schema());
    CopyWriter copier(value);
    while (!copier.done()) {
      Row r(value->get_schema());
      copier.visit(r);
      ret_value->add_row(r);
    }

    this->distro_kv_lock.unlock();

    return ret_value;
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

DataFrame *KeyValueStore::get_local_df(Key &key) {
  assert(this->verify_distributed_layer());
  assert(key.get_home_id() < this->num_nodes);

  if (key.get_home_id() == this->home_node) {
    // Given that all values are distributed, and distributed values are
    // available sent after the distribution of dfs, then we should always
    // have the dataframe associated with the key available
    auto it = this->distro_kv_map.find(&key);
    assert(it != this->distro_kv_map.end());

    return this->distro_kv_map[&key];
  } else {
    return nullptr;
  }
}

void KeyValueStore::local_map(String *key_prefix, Rower &rower) {
  assert(key_prefix);

  this->distro_kv_lock.lock();
  for (auto &iter : this->distro_kv_map) {
    String other_prefix(iter.first->get_name()->c_str(), key_prefix->size());
    if (other_prefix.equals(key_prefix)) {
      // this prefix matches. Map through it with the rower
      DataFrame *df = iter.second;
      df->map(rower);
    }
  }
  this->distro_kv_lock.unlock();
}

LocalNetworkMessageManager *KeyValueStore::connect_local(size_t node_id) {
  assert(this->local_network_layer == nullptr);
  assert(this->real_network_layer == nullptr);
  assert(this->num_nodes > 1);
  assert(this->home_node == -1);

  this->home_node = node_id;

  this->local_network_layer = new LocalNetworkMessageManager(this);
  return this->local_network_layer;
}

void KeyValueStore::register_local(LocalNetworkMessageManager *msg_manager) {
  assert(this->local_network_layer != nullptr);
  this->local_network_layer->register_local(msg_manager);
}

SocketNetworkMessageManager *KeyValueStore::connect_network() {
  assert(this->local_network_layer == nullptr);
  assert(this->real_network_layer == nullptr);
  assert(this->num_nodes > 1);
  assert(this->home_node == -1);

  this->real_network_layer = new SocketNetworkMessageManager(this);
  return this->real_network_layer;
}

size_t KeyValueStore::from_array(Key &key, KeyValueStore *kv, size_t num_values,
                                 float *values) {
  assert(kv && values);
  assert(key.get_home_id() == -1);

  // Create the dataframe
  Schema schema("F");
  auto *ret_value = new DistributedValue(key, schema, kv);

  for (size_t i = 0; i < num_values; i++) {
    Row r(ret_value->get_schema());
    r.set(0, values[i]);
    ret_value->add_row(r);
  }

  ret_value->package_value();

  return num_values;
}

size_t KeyValueStore::from_array(Key &key, KeyValueStore *kv, size_t num_values,
                                 int *values) {
  assert(kv && values);
  assert(key.get_home_id() == -1);

  // Create the dataframe
  Schema schema("I");
  auto *ret_value = new DistributedValue(key, schema, kv);

  for (size_t i = 0; i < num_values; i++) {
    Row r(ret_value->get_schema());
    r.set(0, values[i]);
    ret_value->add_row(r);
  }

  ret_value->package_value();

  return num_values;
}

size_t KeyValueStore::from_array(Key &key, KeyValueStore *kv, size_t num_values,
                                 bool *values) {
  assert(kv && values);
  assert(key.get_home_id() == -1);

  // Create the dataframe
  Schema schema("B");
  auto *ret_value = new DistributedValue(key, schema, kv);

  for (size_t i = 0; i < num_values; i++) {
    Row r(ret_value->get_schema());
    r.set(0, values[i]);
    ret_value->add_row(r);
  }

  ret_value->package_value();

  return num_values;
}

size_t KeyValueStore::from_array(Key &key, KeyValueStore *kv, size_t num_values,
                                 String **values) {
  assert(kv && values);
  assert(key.get_home_id() == -1);

  // Create the dataframe
  Schema schema("S");
  auto *ret_value = new DistributedValue(key, schema, kv);

  for (size_t i = 0; i < num_values; i++) {
    Row r(ret_value->get_schema());
    r.set(0, values[i]);
    ret_value->add_row(r);
  }

  ret_value->package_value();

  return num_values;
}

size_t KeyValueStore::from_scalar(Key &key, KeyValueStore *kv, bool value) {
  assert(kv != nullptr);
  assert(key.get_home_id() == -1);

  // Create the dataframe
  Schema schema("B");
  auto *ret_value = new DistributedValue(key, schema, kv);

  // Store the value
  Row r(ret_value->get_schema());
  r.set(0, value);
  ret_value->add_row(r);

  ret_value->package_value();

  return 1;
}

size_t KeyValueStore::from_scalar(Key &key, KeyValueStore *kv, int value) {
  assert(kv != nullptr);
  assert(key.get_home_id() == -1);

  // Create the dataframe
  Schema schema("I");
  auto *ret_value = new DistributedValue(key, schema, kv);

  // Store the value
  Row r(ret_value->get_schema());
  r.set(0, value);
  ret_value->add_row(r);

  ret_value->package_value();

  return 1;
}

size_t KeyValueStore::from_scalar(Key &key, KeyValueStore *kv, float value) {
  assert(kv != nullptr);
  assert(key.get_home_id() == -1);

  // Create the dataframe
  Schema schema("F");
  auto *ret_value = new DistributedValue(key, schema, kv);

  // Store the value
  Row r(ret_value->get_schema());
  r.set(0, value);
  ret_value->add_row(r);

  ret_value->package_value();

  return 1;
}

size_t KeyValueStore::from_scalar(Key &key, KeyValueStore *kv, String *value) {
  assert(kv != nullptr);
  assert(key.get_home_id() == -1);

  // Create the dataframe
  Schema schema("S");
  auto *ret_value = new DistributedValue(key, schema, kv);

  // Store the value
  Row r(ret_value->get_schema());
  r.set(0, value);
  ret_value->add_row(r);

  ret_value->package_value();

  return 1;
}

bool KeyValueStore::verify_distributed_layer() {
  // Take the home id from the network layer if this application has been
  // configured to dynamically be enumerated.
  if (this->real_network_layer != nullptr) {
    this->home_node = this->real_network_layer->get_home_id();
  }

  return (this->home_node != -1) && (this->home_node < this->num_nodes) &&
         ((this->num_nodes == 1) ||
          ((this->local_network_layer != nullptr) &&
           this->local_network_layer->verify_configuration()) ||
          ((this->real_network_layer != nullptr) &&
           (this->real_network_layer->get_home_id() != -1)));
}

size_t KeyValueStore::get_home_id() { return this->home_node; }

size_t KeyValueStore::from_visitor(Key &key, KeyValueStore *kv,
                                   const char *schema_types, Writer &writer) {
  assert(kv && schema_types);
  assert(key.get_home_id() == -1);

  Schema schema(schema_types);
  auto *ret_value = new DistributedValue(key, schema, kv);

  // Iterate through using the writer
  while (!writer.done()) {
    Row r(schema);
    writer.visit(r);
    ret_value->add_row(r);
  }

  ret_value->package_value();

  return ret_value->nrows();
}

size_t KeyValueStore::from_file(Key &key, KeyValueStore *kv,
                                const char *file_name) {
  assert(kv && file_name);
  assert(key.get_home_id() == -1);

  SorerIntegrator integrator(file_name);
  integrator.parse();
  DistributedValue *value = integrator.convert(key, kv);

  return value->nrows();
}

void KeyValueStore::signal_listener(Key &key) {
  // This should only be available to distributed values
  assert(key.get_home_id() == -1);

  // Get the appropriate lock
  this->listener_lock.lock();
  auto it = this->waiting_listeners.find(&key);
  if (it != this->waiting_listeners.end()) {
    // There's a listener. Signal it
    it->second->notify_all();
  }
  this->listener_lock.unlock();
}

void KeyValueStore::wait_for_listener(Key &key) {
  // This should only be available to distributed values
  assert(key.get_home_id() == -1);

  // Return immediately if we have the value for this key already
  this->kv_lock.lock();
  auto kv_it = this->kv_map.find(&key);
  if (kv_it != this->kv_map.end()) {
    this->kv_lock.unlock();
    return;
  }
  this->kv_lock.unlock();

  // Get a lock to wait on
  Lock *signal = nullptr;
  bool created_signal; // Determines if we own the signal, or we borrowed
  this->listener_lock.lock();
  auto it = this->waiting_listeners.find(&key);
  if (it != this->waiting_listeners.end()) {
    // There's already a listener
    signal = it->second;
    created_signal = false;
  } else {
    // Create a signal and then add it to the listeners
    signal = new Lock();
    created_signal = true;
    this->waiting_listeners[&key] = signal;
  }
  this->listener_lock.unlock();

  // Now wait on the signal
  signal->wait();

  // Got the signal. Now remove it from the listener map if needed
  if (created_signal) {
    this->waiting_listeners.erase(&key);
    delete signal;
  }
}

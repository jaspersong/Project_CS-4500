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
  std::map<Key *, DistributedValue *, KeyComp>::iterator it;
  it = this->kv_map.find(&key);
  while (it == this->kv_map.end()) {
    sleep(1);
    it = this->kv_map.find(&key);
  }

  return this->kv_map[&key];
}

void KeyValueStore::put(Key &key, DistributedValue *value) {
  assert(key.get_home_id() == -1);
  assert(value != nullptr);
  assert(this->verify_distributed_layer());

  // Add this value to the current kv-store
  Key *owned_key = new Key(key.get_name()->c_str(), key.get_home_id());
  this->kv_lock.lock();
  this->kv_map[owned_key] = value;
  this->kv_lock.unlock();
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
    // Wait until the key is available on this key-value store
    std::map<Key *, DataFrame *, KeyComp>::iterator it;
    it = this->distro_kv_map.find(&key);
    while (it == this->distro_kv_map.end()) {
      sleep(1);
      it = this->distro_kv_map.find(&key);
    }

    this->distro_kv_lock.lock();

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
    // Wait until the key is available on this key-value store
    std::map<Key *, DataFrame *, KeyComp>::iterator it;
    it = this->distro_kv_map.find(&key);
    while (it == this->distro_kv_map.end()) {
      sleep(1);
      it = this->distro_kv_map.find(&key);
    }

    return this->distro_kv_map[&key];
  } else {
    return nullptr;
  }
}

void KeyValueStore::local_map(String *key_prefix, Rower &rower) {
  assert(key_prefix);

  this->distro_kv_lock.lock();
  for (auto & iter : this->distro_kv_map) {
    String other_prefix(iter.first->get_name()->c_str(),
                        key_prefix->size());
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

RealNetworkMessageManager *KeyValueStore::connect_network() {
  assert(this->local_network_layer == nullptr);
  assert(this->real_network_layer == nullptr);
  assert(this->num_nodes > 1);
  assert(this->home_node == -1);

  this->real_network_layer = new RealNetworkMessageManager(this);
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

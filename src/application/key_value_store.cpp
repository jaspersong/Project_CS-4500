/**
 * Name: Snowy Chen, Joe Song
 * Date: 21 March 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::Cpp

#include <unistd.h>
#include "key_value_store.h"

KeyValueStore::KeyValueStore(size_t num_nodes) {
  assert(num_nodes > 0);

  this->num_nodes = num_nodes;
  if (this->num_nodes == 1) {
    // This is the only node that this system is expecting. Automatically
    // give it the only available node id.
    this->home_node = 0;
  }
  else {
    // This is one of many nodes. Default to -1, for the registrar, first for
    // now.
    this->home_node = -1;
  }

  this->network_layer = nullptr;
  this->all_apps_registered = false;
}

void KeyValueStore::put(Key &key, DataFrame *value) {
  assert(key.get_home_id() < this->num_nodes);
  assert(value != nullptr);
  assert(this->verify_distributed_layer());

  if (key.get_home_id() == this->home_node) {
    this->kv_lock.lock();
    this->kv_map.put(&key, value);
    this->kv_lock.unlock();
  } else {
    if (this->network_layer == nullptr) {
      // This means that we are connected directly through an application.
      // Just put the value in that application
      DataItem_ item = this->app_list.get_item(key.get_home_id());
      auto *other_kv = reinterpret_cast<KeyValueStore *>(item.o);
      other_kv->put(key, value);
    }
    else {
      // TODO: Connect with the network layer
      assert(false);
    }
  }
}

DataFrame *KeyValueStore::wait_and_get(Key &key) {
  assert(key.get_home_id() < this->num_nodes);
  assert(this->verify_distributed_layer());

  if (key.get_home_id() == this->home_node) {
    this->kv_lock.lock();

    auto *value = reinterpret_cast<DataFrame *>(this->kv_map.get(&key));

    // Now copy the value so it can be owned by the caller.
    auto *ret_value = new DataFrame(*value);
    // TODO: Create a rower/fielder to copy the dataframe into a new
    //  dataframe.

    this->kv_lock.unlock();

//    return ret_value;
    return value;
  }
  else if (key.get_home_id() < this->num_nodes) {
    if (this->network_layer == nullptr) {
      // This means that we are connected directly through an application.
      // Just put the value in that application
      DataItem_ item = this->app_list.get_item(key.get_home_id());
      auto *other_kv = reinterpret_cast<KeyValueStore *>(item.o);

      // Now wait until the key appears in the other kv's storage
      while (!other_kv->kv_map.contains_key(&key)) {
        sleep(5);
      }
      return other_kv->wait_and_get(key);
    }
    else {
      // TODO: Connect with the network layer
      assert(false);
      return nullptr;
    }
  }
  else {
    return nullptr;
  }
}

DataFrame *KeyValueStore::get_local(Key &key) {
  assert(this->verify_distributed_layer());

  if (key.get_home_id() == this->home_node) {
    return reinterpret_cast<DataFrame *>(this->kv_map.get(&key));
  } else {
    return nullptr;
  }
}

void  KeyValueStore::connect_local(size_t node_id) {
  assert(this->network_layer == nullptr);
  assert(this->app_list.size() == 0);
  assert(this->num_nodes > 1);
  assert(this->home_node == -1);

  this->home_node = node_id;

  // Fill the app list with nullptrs to the number of expected nodes to be
  // connected.
  DataItem_ item;
  item.o = nullptr;
  for (size_t i = 0; i < this->num_nodes; i++) {
    this->app_list.add_new_item(item);
  }
}

void KeyValueStore::add_local(KeyValueStore &other_kv) {
  assert(this->network_layer == nullptr);
  assert(this->app_list.size() == this->num_nodes);
  assert(this->num_nodes > 1);
  assert(this->home_node != -1);
  assert(other_kv.home_node != -1);
  assert(other_kv.home_node != this->home_node);
  assert(!this->all_apps_registered);

  // Ensure that this slot has not been taken up by another application
  DataItem_ item = this->app_list.get_item(other_kv.home_node);
  assert(item.o == nullptr);

  // Now add this application to the slot
  item.o = &other_kv;
  this->app_list.set_new_item(other_kv.home_node, item);

  // Check to see if all of the apps have been registered
  bool found_unreg_slot = false;
  for (size_t i = 0; i < this->num_nodes; i++) {
    // No need to have a KV store registered for the node id that shares this
    // app's home node.
    if (i != this->home_node) {
      // See if this slot does not have an app registered to it
      item = this->app_list.get_item(i);
      if (item.o == nullptr) {
        found_unreg_slot = true;
        break;
      }
    }
  }
  this->all_apps_registered = !found_unreg_slot;
}

void KeyValueStore::connect_network(Node &node) {
  assert(this->app_list.size() == 0);
  assert(this->network_layer == nullptr);

  if (this->num_nodes > 1) {
    // There is no need to hook anything up if this is the only node in this
    // distributed app.
    this->network_layer = &node;
    this->home_node = this->network_layer->get_node_id_with_wait();
  }
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
  return (this->home_node != -1) && (this->home_node < this->num_nodes)
    && ((this->num_nodes == 1) || (this->network_layer != nullptr)
    || (this->all_apps_registered));
}

size_t KeyValueStore::get_home_id() {
  assert(this->verify_distributed_layer());

  return this->home_node;
}

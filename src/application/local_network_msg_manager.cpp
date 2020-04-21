/**
 * Name: Snowy Chen, Joe Song
 * Date: 21 March 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::Cpp

#include "local_network_msg_manager.h"
#include "dataframe.h"

LocalNetworkMessageManager::LocalNetworkMessageManager(KeyValueStore *kv_store)
    : ApplicationNetworkInterface(kv_store) {
  // Fill the app list with nullptrs to the number of expected nodes to be
  // connected.
  this->all_apps_registered = false;
  this->app_list.resize(this->kv_store->get_num_nodes(), nullptr);
}

size_t LocalNetworkMessageManager::get_home_id() {
  return this->kv_store->get_home_id();
}

void LocalNetworkMessageManager::send_put(size_t node_id, Key &key,
                                          DataFrame *value) {
  // Build the put command message
  Serializer serializer;
  key.serialize(serializer);
  value->serialize(serializer);
  Put put_message(serializer);

  // Configure the target and sender ids manually when we're not being
  // managed by the network layer
  put_message.set_target_id(node_id);
  put_message.set_sender_id(this->kv_store->get_home_id());

  // This means that we are connected directly through an application.
  // Get the network manager for the target
  auto *other_kv = this->app_list.at(node_id);
  other_kv->handle_put(&put_message);
}

void LocalNetworkMessageManager::send_waitandget(size_t node_id, Key &key) {
  // Build the wait and get message
  Serializer serializer;
  key.serialize(serializer);
  WaitAndGet wait_get_msg(serializer);

  // Configure the target and sender ids manually when we're not being
  // managed by the network layer
  wait_get_msg.set_target_id(node_id);
  wait_get_msg.set_sender_id(this->kv_store->get_home_id());

  // This means that we are connected directly through an application.
  // Request the dataframe
  auto *other_kv = this->app_list.at(node_id);
  other_kv->handle_waitandget(&wait_get_msg);
}

void LocalNetworkMessageManager::send_reply(size_t node_id, Key &key,
                                            DataFrame *df) {
  // Construct the reply
  Serializer serializer;
  key.serialize(serializer);
  df->serialize(serializer);
  auto *reply = new Reply(serializer);

  // Manually add the target and sender id because we aren't being managed
  // by the network layer
  reply->set_sender_id(this->kv_store->get_home_id());
  reply->set_target_id(node_id);

  // Now pass it over to the target network
  auto *other_kv = this->app_list.at(node_id);
  other_kv->handle_reply(reply);
}

void LocalNetworkMessageManager::register_local(
    LocalNetworkMessageManager *msg_manager) {
  assert(this->app_list.size() == this->kv_store->get_num_nodes());
  assert(msg_manager != nullptr);
  assert(msg_manager->get_home_id() != -1);
  assert(msg_manager->get_home_id() != this->kv_store->get_home_id());
  assert(!this->all_apps_registered);

  // Ensure that this slot has not been taken up by another application
  assert(this->app_list.at(msg_manager->get_home_id()) == nullptr);

  // Now add this application to the slot
  this->app_list[msg_manager->get_home_id()] = msg_manager;

  // Check to see if all of the apps have been registered
  bool found_unreg_slot = false;
  for (size_t i = 0; i < this->kv_store->get_num_nodes(); i++) {
    // No need to have a KV store registered for the node id that shares this
    // app's home node.
    if (i != this->kv_store->get_home_id()) {
      // See if this slot does not have an app registered to it
      if (this->app_list.at(i) == nullptr) {
        found_unreg_slot = true;
        break;
      }
    }
  }

  // Update whether or not all the apps are registered
  this->all_apps_registered = !found_unreg_slot;
}
void LocalNetworkMessageManager::broadcast_value(Key &key,
                                                 DistributedValue *value) {
  for (size_t n = 0; n < this->kv_store->get_num_nodes(); n++) {
    // Skip this key-value store
    if (n != this->kv_store->get_home_id()) {
      // Build the put command message
      Serializer serializer;
      key.serialize(serializer);
      value->serialize(serializer);
      Put put_message(serializer);

      // Configure the target and sender ids manually when we're not being
      // managed by the network layer
      put_message.set_target_id(n);
      put_message.set_sender_id(this->kv_store->get_home_id());

      // This means that we are connected directly through an application.
      // Get the network manager for the target
      auto *other_kv = this->app_list.at(n);
      other_kv->handle_put(&put_message);
    }
  }
}

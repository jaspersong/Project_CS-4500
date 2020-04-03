/**
 * Name: Snowy Chen, Joe Song
 * Date: 21 March 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::Cpp

#include "local_network_msg_manager.h"

LocalNetworkMessageManager::LocalNetworkMessageManager(KeyValueStore *kv_store,
                                                       StatusHandler *status_handler)
    : ApplicationNetworkInterface(kv_store, status_handler) {
  // Fill the app list with nullptrs to the number of expected nodes to be
  // connected.
  this->all_apps_registered = false;
  DataItem_ item;
  item.o = nullptr;
  for (size_t i = 0; i < this->kv_store->get_num_nodes(); i++) {
    this->app_list.add_new_item(item);
  }
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
  DataItem_ item = this->app_list.get_item(key.get_home_id());
  auto *other_kv = reinterpret_cast<LocalNetworkMessageManager *>(item.o);
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
  DataItem_ item = this->app_list.get_item(key.get_home_id());
  auto *other_kv = reinterpret_cast<LocalNetworkMessageManager *>(item.o);
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
  DataItem_ item = this->app_list.get_item(node_id);
  auto *other_kv = reinterpret_cast<LocalNetworkMessageManager *>(item.o);
  other_kv->handle_reply(reply);
}

void LocalNetworkMessageManager::send_status(size_t node_id, String &msg) {
  // Construct the reply
  Status status(msg);

  // Manually add the target and sender id because we aren't being managed
  // by the network layer
  status.set_sender_id(this->kv_store->get_home_id());
  status.set_target_id(node_id);

  // Now pass it over to the target network
  DataItem_ item = this->app_list.get_item(node_id);
  auto *other_kv = reinterpret_cast<LocalNetworkMessageManager *>(item.o);
  other_kv->handle_status(&status);
}

void LocalNetworkMessageManager::register_local(LocalNetworkMessageManager *msg_manager) {
  assert(this->app_list.size() == this->kv_store->get_num_nodes());
  assert(msg_manager != nullptr);
  assert(msg_manager->get_home_id() != -1);
  assert(msg_manager->get_home_id() != this->kv_store->get_home_id());
  assert(!this->all_apps_registered);

  // Ensure that this slot has not been taken up by another application
  DataItem_ item = this->app_list.get_item(msg_manager->get_home_id());
  assert(item.o == nullptr);

  // Now add this application to the slot
  item.o = msg_manager;
  this->app_list.set_new_item(msg_manager->get_home_id(), item);

  // Check to see if all of the apps have been registered
  bool found_unreg_slot = false;
  for (size_t i = 0; i < this->kv_store->get_num_nodes(); i++) {
    // No need to have a KV store registered for the node id that shares this
    // app's home node.
    if (i != this->kv_store->get_home_id()) {
      // See if this slot does not have an app registered to it
      item = this->app_list.get_item(i);
      if (item.o == nullptr) {
        found_unreg_slot = true;
        break;
      }
    }
  }

  // Update whether or not all the apps are registered
  this->all_apps_registered = !found_unreg_slot;
}

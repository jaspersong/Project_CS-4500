/**
 * Name: Snowy Chen, Joe Song
 * Date: 29 March 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::Cpp

#include "networked_msg_manager.h"

RealNetworkMessageManager::RealNetworkMessageManager(KeyValueStore *kv_store,
                                                     StatusHandler *status_handler)
    : ApplicationNetworkInterface(kv_store, status_handler) {
  this->network_layer = nullptr;
}

void RealNetworkMessageManager::set_client(Client *client) {
  assert(this->network_layer == nullptr);
  this->network_layer =reinterpret_cast<Node *>(client);
}

size_t RealNetworkMessageManager::get_home_id() {
  if (this->network_layer != nullptr) {
    return this->network_layer->get_node_id_with_wait();
  }
  else {
    return -1;
  }
}

void RealNetworkMessageManager::send_put(size_t node_id, Key &key,
                                          DataFrame *value) {
  // Build the put command message
  Serializer serializer;
  key.serialize(serializer);
  value->serialize(serializer);
  Put put_message(serializer);

  this->network_layer->send_direct_message(node_id, put_message);
}

void RealNetworkMessageManager::send_waitandget(size_t node_id, Key &key) {
  // Build the wait and get message
  Serializer serializer;
  key.serialize(serializer);
  WaitAndGet wait_get_msg(serializer);

  this->network_layer->send_direct_message(node_id, wait_get_msg);
}

void RealNetworkMessageManager::send_reply(size_t node_id, Key &key,
                                            DataFrame *df) {
  // Construct the reply
  Serializer serializer;
  key.serialize(serializer);
  df->serialize(serializer);
  Reply reply(serializer);

  this->network_layer->send_direct_message(node_id, reply);
}

void RealNetworkMessageManager::send_status(size_t node_id, String &msg) {
  // Construct the reply
  Status status(msg);

  this->network_layer->send_direct_message(node_id, status);
}
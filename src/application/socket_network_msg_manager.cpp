/**
 * Name: Snowy Chen, Joe Song
 * Date: 29 March 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::Cpp

#include "socket_network_msg_manager.h"
#include "dataframe.h"

SocketNetworkMessageManager::SocketNetworkMessageManager(
    KeyValueStore *kv_store)
    : ApplicationNetworkInterface(kv_store) {
  this->network_layer = nullptr;
}

void SocketNetworkMessageManager::set_network(SocketNetwork *network) {
  assert(this->network_layer == nullptr);
  this->network_layer = network;
}

size_t SocketNetworkMessageManager::get_home_id() {
  if (this->network_layer != nullptr) {
    this->network_layer->wait_for_all_connected();
    return this->network_layer->get_id();
  } else {
    return -1;
  }
}

void SocketNetworkMessageManager::send_put(size_t node_id, Key &key,
                                           DataFrame *value) {
  // Build the put command message
  Serializer serializer;
  key.serialize(serializer);
  value->serialize(serializer);
  Put put_message(serializer);

  this->network_layer->send_message(node_id, put_message);
}

void SocketNetworkMessageManager::send_waitandget(size_t node_id, Key &key) {
  // Build the wait and get message
  Serializer serializer;
  key.serialize(serializer);
  WaitAndGet wait_get_msg(serializer);

  this->network_layer->send_message(node_id, wait_get_msg);
}

void SocketNetworkMessageManager::send_reply(size_t node_id, Key &key,
                                             DataFrame *df) {
  // Construct the reply
  Serializer serializer;
  key.serialize(serializer);
  df->serialize(serializer);
  Reply reply(serializer);

  this->network_layer->send_message(node_id, reply);
}
void SocketNetworkMessageManager::broadcast_value(Key &key,
                                                  DistributedValue *value) {
  // Build the put command message
  Serializer serializer;
  key.serialize(serializer);
  value->serialize(serializer);
  Put put_message(serializer);

  this->network_layer->broadcast_message(put_message);
}

/**
 * Name: Snowy Chen, Joe Song
 * Date: 29 March 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::Cpp

#pragma once

#include "application_network_interface.h"
#include "distributed_app_network_layer.h"

/**
 * An implementation of an interface between an application and a network
 * layer so that the application is able to interface over a distributed
 * application using a real network layer.
 */
class SocketNetworkMessageManager : public ApplicationNetworkInterface {
public:
  explicit SocketNetworkMessageManager(KeyValueStore *kv_store);

  // Attaches the network layer to this interface for an application.
  void set_network(SocketNetwork *network) override;

  void send_put(size_t node_id, Key &key, DataFrame *value) override;
  void send_waitandget(size_t node_id, Key &key) override;
  void send_reply(size_t node_id, Key &key, DataFrame *df) override;
  void broadcast_value(Key &key, DistributedValue *value) override;

  // This is only available if a network layer has been attached to this
  // interface.
  size_t get_home_id() override;

private:
  SocketNetwork *network_layer;
};

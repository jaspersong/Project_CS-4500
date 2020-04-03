/**
 * Name: Snowy Chen, Joe Song
 * Date: 29 March 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::Cpp

#pragma once

#include "key_value_store.h"
#include "thread.h"
#include "application_network_interface.h"

/**
 * An implementation of an interface between an application and a network
 * layer so that the application is able to interface over a distributed
 * application using a real network layer.
 */
class RealNetworkMessageManager : public ApplicationNetworkInterface {
public:
  RealNetworkMessageManager(KeyValueStore *kv_store, StatusHandler *status_handler);

  // Attaches a network layer to this interface for an application. Only a
  // Node is a valid client that can be attached to this interface. If a
  // non-Node client is attached to this interface, it will do nothing.
  void set_client(Client *client) override;

  void send_put(size_t node_id, Key &key, DataFrame *value) override;
  void send_waitandget(size_t node_id, Key &key) override;
  void send_reply(size_t node_id, Key &key, DataFrame *df) override;
  void send_status(size_t node_id, String &msg) override;

  // This is only available if a network layer has been attached to this
  // interface.
  size_t get_home_id() override;

private:
  Node *network_layer;
};

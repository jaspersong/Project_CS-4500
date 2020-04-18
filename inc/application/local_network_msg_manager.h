/**
 * Name: Snowy Chen, Joe Song
 * Date: 21 March 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::Cpp

#pragma once

#include "key_value_store.h"
#include "application_network_interface.h"

/**
 * An implementation of an interface between an application and a network
 * layer so that the application is able to interface over a distributed
 * application using a fake network.
 */
class LocalNetworkMessageManager : public ApplicationNetworkInterface {
public:
  explicit LocalNetworkMessageManager(KeyValueStore *kv_store);

  /**
   * Registers the provided message manager to other applications. This MUST
   * only be called AFTER connect_local() has been called.
   * @param msg_manager The message manager provided by another application
   * that is configured to connect to local networks.
   */
  void register_local(LocalNetworkMessageManager *msg_manager);

  /**
   * Determines whether or not the local network has finished configuring and
   * will be able to run properly for the application.
   * @return True if the configuration has been completed. False if otherwise.
   */
  bool verify_configuration() { return this->all_apps_registered; }

  void send_put(size_t node_id, Key &key, DataFrame *value) override;
  void send_waitandget(size_t node_id, Key &key) override;
  void send_reply(size_t node_id, Key &key, DataFrame *df) override;
  void broadcast_value(Key &key, DistributedValue *value) override;

  size_t get_home_id() override;

private:
  std::vector<LocalNetworkMessageManager *> app_list;
  bool all_apps_registered;
};

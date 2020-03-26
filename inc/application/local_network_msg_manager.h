/**
 * Name: Snowy Chen, Joe Song
 * Date: 21 March 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::Cpp

#pragma once

#include "key_value_store.h"
#include "recv_msg_manager.h"
#include "thread.h"

class KeyValueStore;

/**
 * An implementation of message manager that can be used to communicate over
 * a fake network layer. It is used to link a distributed key-value store
 * that are different threads running on the same process
 */
class LocalNetworkMessageManager : public ReceivedMessageManager {
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

  bool handle_put(Put &msg) override;
  bool handle_waitandget(WaitAndGet &msg) override;
  // The reply passed in should be dynamically allocated
  bool handle_reply(Reply &msg) override;

  /**
   * Sends the messages of the specified type with the provided content.
   */
  void send_put(size_t node_id, Key &key, DataFrame *value);
  void send_waitandget(size_t node_id, Key &key);
  void send_reply(size_t node_id, Key &key, DataFrame *df);

  DataFrame *get_requested_dataframe();

  size_t get_home_id();

private:
  KeyValueStore *kv_store;
  Queue reply_queue;
  ArrayOfArrays app_list;
  bool all_apps_registered;

  void wait_for_reply() override;
  Reply *get_reply() override;
};

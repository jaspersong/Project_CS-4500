/**
 * Name: Snowy Chen, Joe Song
 * Date: 21 March 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::Cpp

#pragma once

#include "recv_msg_manager.h"
#include "key_value_store.h"
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

  bool handle_put(Put &msg) override;
  bool handle_waitandget(WaitAndGet &msg) override;
  // The reply passed in should be dynamically allocated
  bool handle_reply(Reply &msg) override;

  DataFrame *get_requested_dataframe();

  size_t get_home_id();
private:
  KeyValueStore *kv_store;
  Queue reply_queue;

  void wait_for_reply() override;
  Reply *get_reply() override;
};

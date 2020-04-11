/**
 * Name: Snowy Chen, Joe Song
 * Date: 29 March 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::Cpp

#pragma once

#include "key_value_store.h"
#include "recv_msg_manager.h"
#include "queue.h"

/**
 * An implementation of the received message manager that can be used to
 * communicate over a specified network layer that is configured by
 * implementing the virtual functions.
 */
class ApplicationNetworkInterface : public ReceivedMessageManager {
public:
  explicit ApplicationNetworkInterface(KeyValueStore *kv_store);

  bool handle_put(Put *msg) override;
  bool handle_waitandget(WaitAndGet *msg) override;
  bool handle_reply(Reply *msg) override;

  /**
   * Sends the messages of the specified type with the provided content.
   */
  virtual void send_put(size_t node_id, Key &key, DataFrame *value) {
    assert(false);
  }

  virtual void send_waitandget(size_t node_id, Key &key) {
    assert(false);
  }

  virtual void broadcast_value(Key &key, DistributedValue *value) {
    assert(false);
  }

  virtual void send_reply(size_t node_id, Key &key, DataFrame *df) {
    assert(false);
  }

  DataFrame *get_requested_dataframe();

  virtual size_t get_home_id();

protected:
  KeyValueStore *kv_store;
  Queue reply_queue;

  void wait_for_reply() override;
  Reply *get_reply() override;
};

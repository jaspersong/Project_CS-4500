/**
 * Name: Snowy Chen, Joe Song
 * Date: 29 March 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::Cpp

#include "application_network_interface.h"
#include "distributed_value.h"
#include "dataframe.h"

ApplicationNetworkInterface::ApplicationNetworkInterface(
    KeyValueStore *kv_store) {
  assert(kv_store != nullptr);
  this->kv_store = kv_store;
}

bool ApplicationNetworkInterface::handle_put(Put *msg) {
  assert(this->kv_store->verify_distributed_layer());

  // Get the key and the dataframe
  Deserializer *deserializer = msg->steal_deserializer();
  Key *key = Key::deserialize_as_key(*deserializer);

  // Determine how this value will be put into th key-value store
  if (key->get_home_id() == -1) {
    DistributedValue *value =
        DistributedValue::deserialize_as_distributed_val(*deserializer, *key,
            this->kv_store);
    this->kv_store->put(*key, value);
  } else if (key->get_home_id() == this->kv_store->get_home_id()) {
    DataFrame *df = DataFrame::deserialize_as_dataframe(*deserializer);
    this->kv_store->put_df(*key, df);
  } else {
    // Should not get here.
    assert(false);
  }

  delete key;
  delete deserializer;

  return false;
}

bool ApplicationNetworkInterface::handle_waitandget(WaitAndGet *msg) {
  assert(this->kv_store->verify_distributed_layer());

  Deserializer *deserializer = msg->steal_deserializer();
  Key *key = Key::deserialize_as_key(*deserializer);

  // Key cannot be asking for a distributed value, since they should be
  // broadcasted
  assert(key->get_home_id() != -1);

  // Get the requested value and send it as a response
  DataFrame *df = this->kv_store->get_local_df(*key);
  this->send_reply(msg->get_sender_id(), *key, df);

  delete key;
  delete deserializer;
  return false;
}

bool ApplicationNetworkInterface::handle_reply(Reply *msg) {
  assert(this->kv_store->verify_distributed_layer());

  this->reply_queue_lock.lock();
  this->reply_queue.push(msg);
  this->reply_queue_lock.unlock();

  this->reply_signal.notify_all();
  return true;
}

void ApplicationNetworkInterface::wait_for_reply() {
  assert(this->kv_store->verify_distributed_layer());

  this->reply_queue_lock.lock();
  if (this->reply_queue.empty()) {
    this->reply_queue_lock.unlock();
    this->reply_signal.wait();
  } else {
    this->reply_queue_lock.unlock();
  }
}

Reply *ApplicationNetworkInterface::get_reply() {
  assert(this->kv_store->verify_distributed_layer());

  this->reply_queue_lock.lock();
  Reply * ret_value = this->reply_queue.front();
  this->reply_queue.pop();
  this->reply_queue_lock.unlock();
  return ret_value;
}

DataFrame *ApplicationNetworkInterface::get_requested_dataframe() {
  assert(this->kv_store->verify_distributed_layer());

  this->wait_for_reply();
  Reply *reply = this->get_reply();

  // Get the data
  Deserializer *deserializer = reply->steal_deserializer();
  Key *key = Key::deserialize_as_key(*deserializer);
  DataFrame *df = DataFrame::deserialize_as_dataframe(*deserializer);

  // TODO: Verify the key matches to the key that we were expecting. For now,
  //  we will assume that it's correct.

  delete key;
  delete deserializer;
  delete reply;

  return df;
}

size_t ApplicationNetworkInterface::get_home_id() {
  assert(this->kv_store->verify_distributed_layer());
  return this->kv_store->get_home_id();
}

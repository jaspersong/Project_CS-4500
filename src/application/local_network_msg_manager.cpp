/**
 * Name: Snowy Chen, Joe Song
 * Date: 21 March 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::Cpp

#include "local_network_msg_manager.h"

LocalNetworkMessageManager::LocalNetworkMessageManager(KeyValueStore *kv_store) {
  assert(kv_store != nullptr);
  this->kv_store = kv_store;
}

bool LocalNetworkMessageManager::handle_put(Put &msg) {
  // Get the key and the dataframe
  Deserializer *deserializer = msg.steal_deserializer();
  Key *key = Key::deserialize_as_key(*deserializer);
  DataFrame *df = DataFrame::deserialize_as_dataframe(*deserializer);

  // TODO: Ensure that this key and message has arrived to the intended
  //  destination
  this->kv_store->put(*key, df);
  // TODO: There's a memory leak once the key-value store gets deconstructed.

  delete deserializer;

  return true;
}

bool LocalNetworkMessageManager::handle_waitandget(WaitAndGet &msg) {
  Deserializer *deserializer = msg.steal_deserializer();
  Key *key = Key::deserialize_as_key(*deserializer);

  // Get the requested dataframe
  DataFrame *df = this->kv_store->get_local(*key);

  // Send the reply
  this->kv_store->reply(msg.get_sender_id(), key, df);

  delete key;
  return true;
}

bool LocalNetworkMessageManager::handle_reply(Reply &msg) {
  this->reply_queue.enqueue(&msg);
  return true;
}

void LocalNetworkMessageManager::wait_for_reply() {
  this->reply_queue.wait_for_items();
}

Reply *LocalNetworkMessageManager::get_reply() {
  return reinterpret_cast<Reply *>(this->reply_queue.dequeue());
}

DataFrame *LocalNetworkMessageManager::get_requested_dataframe() {
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

size_t LocalNetworkMessageManager::get_home_id() {
  return this->kv_store->get_home_id();
}

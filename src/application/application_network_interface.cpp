/**
 * Name: Snowy Chen, Joe Song
 * Date: 29 March 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::Cpp

#include "application_network_interface.h"

ApplicationNetworkInterface::ApplicationNetworkInterface(
    KeyValueStore *kv_store, StatusHandler *status_handler) {
  assert(kv_store != nullptr);
  this->kv_store = kv_store;
  this->status_handler = status_handler;
}

bool ApplicationNetworkInterface::handle_put(Put *msg) {
  // Get the key and the dataframe
  Deserializer *deserializer = msg->steal_deserializer();
  Key *key = Key::deserialize_as_key(*deserializer);
  DataFrame *df = DataFrame::deserialize_as_dataframe(*deserializer);

  // TODO: Ensure that this key and message has arrived to the intended
  //  destination
  this->kv_store->put(*key, df);

  delete key;
  delete deserializer;

  return false;
}

bool ApplicationNetworkInterface::handle_waitandget(WaitAndGet *msg) {
  Deserializer *deserializer = msg->steal_deserializer();
  Key *key = Key::deserialize_as_key(*deserializer);

  // Get the requested dataframe and send it it as the response
  DataFrame *df = this->kv_store->get_local(*key);
  this->send_reply(msg->get_sender_id(), *key, df);

  delete key;
  delete deserializer;
  return false;
}

bool ApplicationNetworkInterface::handle_reply(Reply *msg) {
  this->reply_queue.enqueue(msg);
  return true;
}

bool ApplicationNetworkInterface::handle_status(Status *msg) {
  if (this->status_handler != nullptr) {
    return this->status_handler->handle_status(msg);
  }
  else {
    return false;
  }
}

void ApplicationNetworkInterface::wait_for_reply() {
  this->reply_queue.wait_for_items();
}

Reply *ApplicationNetworkInterface::get_reply() {
  return reinterpret_cast<Reply *>(this->reply_queue.dequeue());
}

DataFrame *ApplicationNetworkInterface::get_requested_dataframe() {
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
  return this->kv_store->get_home_id();
}

/**
 * Name: Snowy Chen, Joe Song
 * Date: 21 March 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::Cpp

#include "stdout_msg_manager.h"

StdoutMessageManager::StdoutMessageManager(size_t std_id) {
  this->std_id = std_id;
}

bool StdoutMessageManager::handle_put(Put *msg) {
  (void)msg;
  printf("%zu: Received a put message from %zu.\n", this->std_id,
         msg->get_sender_id());
  return false;
}

bool StdoutMessageManager::handle_waitandget(WaitAndGet *msg) {
  (void)msg;
  printf("%zu: Received a Wait and Get message from %zu.\b", this->std_id,
         msg->get_sender_id());
  return false;
}

bool StdoutMessageManager::handle_status(Status *msg) {
  String *status_message = msg->get_message();
  printf("%zu: Received a status message from %zu: %s\n", this->std_id,
         msg->get_sender_id(), status_message->c_str());

  return false;
}

bool StdoutMessageManager::handle_reply(Reply *msg) {
  (void)msg;
  printf("%zu: Received a Reply message.\b", this->std_id);
  return false;
}

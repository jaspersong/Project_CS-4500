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

void StdoutMessageManager::handle_ack(Ack &msg) {
  printf("%zu: Received ack message.\n", this->std_id);
}

void StdoutMessageManager::handle_nack(Nack &msg) {
  printf("%zu: Received Nack message.\n", this->std_id);
}

bool StdoutMessageManager::handle_put(Put &msg) {
  printf("%zu: Received a put message.\n", this->std_id);
  return true;
}

bool StdoutMessageManager::handle_get(Get &msg) {
  printf("%zu: Received a get message.\n", this->std_id);
  return true;
}

bool StdoutMessageManager::handle_waitandget(WaitAndGet &msg) {
  printf("%zu: Received a Wait and Get message.\b", this->std_id);
  return true;
}

bool StdoutMessageManager::handle_status(Status &msg) {
  String *status_message = msg.get_message();
  printf("%zu: Received a status message: %s\n", this->std_id,
         status_message->c_str());
  delete status_message;

  return true;
}

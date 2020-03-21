/**
 * Name: Snowy Chen, Joe Song
 * Date: 21 February 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::CwC

#include "recv_msg_manager.h"

/**
 * An implementation of TCP message manager that simply puts a notification
 * on the stdout.
 */
class StdoutMessageManager : public ReceivedMessageManager {
public:
  size_t std_id;

  explicit StdoutMessageManager(size_t std_id) { this->std_id = std_id; }

  void handle_ack(Ack &msg) override {
    printf("%zu: Received ack message.\n", this->std_id);
  }

  void handle_nack(Nack &msg) override {
    printf("%zu: Received Nack message.\n", this->std_id);
  }

  bool handle_put(Put &msg) override {
    printf("%zu: Received a put message.\n", this->std_id);
    return true;
  }

  bool handle_get(Get &msg) override {
    printf("%zu: Received a get message.\n", this->std_id);
    return true;
  }

  bool handle_waitandget(WaitAndGet &msg) override {
    printf("%zu: Received a Wait and Get message.\b", this->std_id);
    return true;
  }

  bool handle_status(Status &msg) override {
    String *status_message = msg.get_message();
    printf("%zu: Received a status message: %s\n", this->std_id,
           status_message->c_str());
    delete status_message;

    return true;
  }
};

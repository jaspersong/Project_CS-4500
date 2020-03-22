/**
 * Name: Snowy Chen, Joe Song
 * Date: 21 March 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::Cpp

#pragma once

#include "recv_msg_manager.h"

/**
 * An implementation of TCP message manager that simply puts a notification
 * on the stdout.
 */
class StdoutMessageManager : public ReceivedMessageManager {
public:
  explicit StdoutMessageManager(size_t std_id);

  void handle_ack(Ack &msg) override;
  void handle_nack(Nack &msg) override;
  bool handle_put(Put &msg) override;
  bool handle_get(Get &msg) override;
  bool handle_waitandget(WaitAndGet &msg) override;
  bool handle_status(Status &msg) override;

private:
  size_t std_id;
};

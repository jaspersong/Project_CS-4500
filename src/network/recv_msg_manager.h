/**
 * Name: Snowy Chen, Joe Song
 * Date: 21 February 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::CwC

#pragma once

#include "thread.h"
#include "serial.h"
#include "network.h"

/**
 * An abstract class that manages the messages received by the particular
 * server/client that use the message types in the serial.h.
 */
class ReceivedMessageManager {
public:
  /**
   * Virtual functions that can be overridden in order to handle generalized
   * messages with varying payloads.
   * @param msg The message received.
   */
  virtual void handle_ack(Ack &msg) {}
  virtual void handle_nack(Nack &msg) {}

  /**
   * Virtual functions that can be overridden in order to handle generalized
   * messages with varying payloads.
   * @param msg The message received.
   * @return True if the message payload is valid, false if otherwise.
   */
  virtual bool handle_put(Put &msg) { return true; }
  virtual bool handle_get(Get &msg) { return true; }
  virtual bool handle_waitandget(WaitAndGet &msg) { return true; }
  virtual bool handle_status(Status &msg) { return true; }
};

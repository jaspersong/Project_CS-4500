/**
 * Name: Snowy Chen, Joe Song
 * Date: 21 February 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::CwC

#pragma once

#include "serial.h"

class SocketNetwork;

/**
 * An abstract class that manages the messages received by a registrar or node
 * instance socket network instance.
 */
class ReceivedMessageManager : public CustomObject {
public:
  /**
   * Virtual functions that can be overridden in order to handle generalized
   * messages with varying payloads.
   * @param msg The message received.
   * @return True if the message has been taken/stolen by the function, and
   * is no longer owned by the caller of the function.
   */
  virtual bool handle_reply(Reply *msg) { return false; }
  virtual bool handle_put(Put *msg) { return false; }
  virtual bool handle_waitandget(WaitAndGet *msg) { return false; }
  virtual bool handle_status(Status *msg) { return false; }

  virtual void set_network(SocketNetwork *network) {}

  /**
   * Virtual functions that can wait for a reply to a wait and get message,
   * and then retrieve the reply that was received. Once get_reply() gets
   * called, then the reply has been consumed and the same reply cannot be
   * retrieved.
   */
  virtual void wait_for_reply() {}
  // If there is no reply available, it will return with nullptr
  virtual Reply *get_reply() { return nullptr; }
};

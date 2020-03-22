/**
 * Name: Snowy Chen, Joe Song
 * Date: 21 March 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::Cpp

#pragma once

#include "key_value_store.h"
#include "thread.h"

/**
 * An interface that details the API functions needed for an application that
 * will be run on the server of the eau2 system.
 */
class Application : public Thread {
public:
  explicit Application(size_t node_id);

  /**
   * The main function of the application.
   */
  void run() override;

  /**
   * Gets the node id that the application is running on.
   * @return The node id that the application is running on. -1 if it is running
   * on the server.
   */
  size_t get_node_id();

protected:
  KeyValueStore *kv;
  size_t node_id;
};

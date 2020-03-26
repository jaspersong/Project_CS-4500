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
  explicit Application(size_t num_nodes);

  /**
   * The main function of the application.
   */
  virtual void main();

  /**
   * Gets the node id that the application is running on.
   * @return The node id that the application is running on. -1 if it is running
   * on the server.
   */
  size_t get_node_id();

  bool is_running() { return this->running; }

  /**
   * Configures this application to other application instances that are
   * running on a different thread within the same process. connect_local()
   * can only be called once in order to configure this KVStore to
   * communicate with other locally distributed applications. In order to
   * formally hook the application to other applications, call register_local()
   * with the returned message manager on the other application instances.
   * @param node_id The node id that this application will now be running on.
   * @return The configured message manager that can be provided to
   * register_local() called on other application instances in order to allow
   * other applications to communicate to this application instance.
   */
  LocalNetworkMessageManager *connect_local(size_t node_id);

  /**
   * Registers the provided message manager to other applications. This MUST
   * only be called AFTER connect_local() has been called.
   * @param msg_manager The message manager provided by another application
   * that is configured to connect to local networks.
   */
  void register_local(LocalNetworkMessageManager *msg_manager);

protected:
  KeyValueStore *kv;
  bool running;

  void run() override;
};

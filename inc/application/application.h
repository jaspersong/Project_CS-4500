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
   * Connects this application to other application instances that are
   * readily available over a different thread, but in the same process.
   * connect_local can only be called once in order to configure this KVStore
   * to communicate with other locally distributed applications. Call
   * add_local() to add multiple local applications that also have had
   * connect_local() called to hook up to. add_local() is a directed
   * connection, so in order to establish mutual connection, add_local() must
   * be called on both applications.
   *
   * However, this cannot be called if connect_network() has been
   * called. This function should be called only if the application isn't
   * already running. In addition, this function or connect_network() must be called
   * before running the application, unless the application is expecting to have
   * only one instance (num_nodes in the constructor is 1).
   * @param app The application that this application will link to.
   */
  void connect_local(size_t node_id);
  void add_local(Application &other_app);

  /**
   * Connects this application to a network node so that it will communicate
   * with other application instances over the network layer. This function
   * can only be called once. This cannot be called if connect_local() has
   * been called. In addition, this function or connect_network() must be
   * called before running the application, unless the application is
   * expecting to have only one instance (num_nodes in the constructor is 1).
   */
  void connect_network(Node &node);

protected:
  KeyValueStore *kv;
  bool running;

  void run() override;
};

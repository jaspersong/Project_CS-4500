/**
 * Name: Snowy Chen, Joe Song
 * Date: 21 March 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::Cpp

#pragma once

#include "dataframe.h"
#include "distributed_app.h"
#include "key.h"
#include "map.h"
#include "thread.h"

class Application;

class KeyValueStore : public CustomObject {
public:
  explicit KeyValueStore(size_t num_nodes);

  /**
   * Puts the dataframe to the associated key. If the key-value store is not the
   * one that is supposed to store this dataframe, it will give the dataframe to
   * the correct home node. Once the value has been passed to the function,
   * the KV-store will now own the value.
   * @param key The key that this value will be associated with
   * @param value The value that will be stored
   */
  void put(Key &key, DataFrame *value);

  /**
   * Wait and gets the associated value of the key.
   * @param key The key that the desired value is associated with.
   * @return The dataframe value associated with the key. The dataframe will be
   * dynamically allocated the caller will now own the dataframe. This can be
   * called for locally stored dataframes. If the key cannot be found, it
   * will return nullptr.
   */
  DataFrame *wait_and_get(Key &key);

  /**
   * Gets a local dataframe stored within this KV-store.
   * @param key The key that the desired value is associated with.
   * @return The dataframe that is locally stored. The dataframe not owned by
   * the caller, and should not be mutated. This is intended only to have the
   * Dataframe read.
   */
  DataFrame *get_local(Key &key);

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
  void add_local(KeyValueStore &other_kv);

  /**
   * Connects this application to a network node so that it will communicate
   * with other application instances over the network layer. This function
   * can only be called once. This cannot be called if connect_local() has
   * been called. In addition, this function or connect_network() must be
   * called before running the application, unless the application is
   * expecting to have only one instance (num_nodes in the constructor is 1).
   */
  void connect_network(Node &node);

  /**
   * Verifies that the distributed layer has been configured properly in
   * order for the KV-store to properly get/put dataframes at the correct
   * distributed KV-Store
   * @return True if the layer has been configured properly. False if otherwise.
   */
  bool verify_distributed_layer();

  /**
   * Generates a Dataframe out of the provided array, and then stores it
   * within the map at the specified key.
   * @param key The key associated with the new dataframe.
   * @param kv The map that will store the dataframe at the specified key.
   * @param num_values The number of values of the provided array.
   * @param values The array of valuas that will be used to create a dataframe
   * @return The newly created dataframe
   */
  static DataFrame *from_array(Key &key, KeyValueStore *kv, size_t num_values,
                               float *values);
  static DataFrame *from_array(Key &key, KeyValueStore *kv, size_t num_values,
                               int *values);
  static DataFrame *from_array(Key &key, KeyValueStore *kv, size_t num_values,
                               bool *values);
  static DataFrame *from_array(Key &key, KeyValueStore *kv, size_t num_values,
                               String **values);

  /**
   * Generates a Dataframe value, and then stores it within the map at the
   * specified key.
   * @param key The key associated with the new dataframe.
   * @param map The map that will store the dataframe at the specified key.
   * @param value The value that the dataframe will store.
   * @return The newly created dataframe
   */
  static DataFrame *from_scalar(Key &key, KeyValueStore *kv, bool value);
  static DataFrame *from_scalar(Key &key, KeyValueStore *kv, int value);
  static DataFrame *from_scalar(Key &key, KeyValueStore *kv, float value);
  static DataFrame *from_scalar(Key &key, KeyValueStore *kv, String *value);

  /**
   * Gets the home node id of this KV store ONLY if the distributed
   * application layer has already been configured.
   * @return The home node id.
   * @throws Throws an error and terminates the program if the distributed
   * application layer has not been properly configured.
   */
  size_t get_home_id();

private:
  Map kv_map;
  size_t home_node; // The id of the node this keyvalue store is running on
  size_t num_nodes;

  Lock kv_lock;

  Node *network_layer;
  ArrayOfArrays app_list;
  bool all_apps_registered;
};

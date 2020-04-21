/**
 * Name: Snowy Chen, Joe Song
 * Date: 21 March 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::Cpp

#pragma once

#include <map>
#include "thread.h"
#include "key.h"
#include "distributed_value.h"
#include "rower.h"

class LocalNetworkMessageManager;
class SocketNetworkMessageManager;

class KeyValueStore : public CustomObject {
public:
  explicit KeyValueStore(size_t num_nodes);
  ~KeyValueStore() override;

  /**
   * Gets the home node id of this KV store ONLY if the distributed
   * application layer has already been configured.
   * @return The home node id.
   * @throws Throws an error and terminates the program if the distributed
   * application layer has not been properly configured.
   */
  size_t get_home_id();

  size_t get_num_nodes() { return this->num_nodes; }

  /**
   * Puts the value to the associated key. Once the value has been passed to the
   * function, the KV-store will now own the value.
   * @param key The key that this value will be associated with
   * @param value The value that will be stored. All dataframes put in the
   * kv-store will be owned by the kvstore
   */
  void put(Key &key, DistributedValue *value);

  /**
   * Wait and gets the associated value of the key. The value will remain
   * owned by the key-value store.
   * @param key The key that the desired value is associated with.
   * @return The dataframe value associated with the key. The dataframe will be
   * dynamically allocated the caller will now own the dataframe. This can be
   * called for locally stored dataframes.
   */
  DistributedValue *wait_and_get(Key &key);

  /**
   * Functions that handle distribution of the dataframe chunks of a
   * distributed value. These functions should only be called by
   * DistributedValue, Key, and the ApplicationNetworkInterface classes.
   */
  void local_map(String *key_prefix, Rower &rower);
  void put_df(Key &key, DataFrame *value);
  DataFrame *wait_and_get_df(Key &key);
  DataFrame *get_local_df(Key &key);
  void broadcast_value(Key &key, DistributedValue *value);

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

  /**
   * Configures this application to use a real network layer.
   */
  SocketNetworkMessageManager *connect_network();

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
   * @return The number of rows of the dataframe created
   */
  static size_t from_array(Key &key, KeyValueStore *kv, size_t num_values,
                           float *values);
  static size_t from_array(Key &key, KeyValueStore *kv, size_t num_values,
                           int *values);
  static size_t from_array(Key &key, KeyValueStore *kv, size_t num_values,
                           bool *values);
  static size_t from_array(Key &key, KeyValueStore *kv, size_t num_values,
                           String **values);

  /**
   * Generates a Dataframe value, and then stores it within the map at the
   * specified key.
   * @param key The key associated with the new dataframe.
   * @param map The map that will store the dataframe at the specified key.
   * @param value The value that the dataframe will store.
   * @return The number of rows of the dataframe created
   */
  static size_t from_scalar(Key &key, KeyValueStore *kv, bool value);
  static size_t from_scalar(Key &key, KeyValueStore *kv, int value);
  static size_t from_scalar(Key &key, KeyValueStore *kv, float value);
  static size_t from_scalar(Key &key, KeyValueStore *kv, String *value);

  /**
   * Generates a dataframe using a visitor and storing it with the key at the
   * key-value store.
   * @param key The key to store the generated dataframe with.
   * @param kv The key-value store to store the dataframe
   * @param schema_types The schema type of the dataframe
   * @param writer The writer visitor that will build the dataframe
   * @return The number of rows of the dataframe created
   */
  static size_t from_visitor(Key &key, KeyValueStore *kv,
                             const char *schema_types, Writer &writer);

  /**
   * Generates a dataframe using a visitor and storing it with the key into
   * the key-value store
   * @param key  The key to store the generated dataframe with
   * @param kv The key-value store to store the dataframe
   * @param file_name The name of the file to read from in order to get a
   * dataframe
   * @return The number of rows of the dataframe created
   */
  static size_t from_file(Key &key, KeyValueStore *kv, const char *file_name);

private:
  class KeyComp {
  public:
    bool operator()(const Key *lhs, const Key *rhs) const {
      // Ensure that all of the map keys have keys that live in the same home id
      assert(lhs->get_home_id() == rhs->get_home_id());

      return strcmp(lhs->get_name()->c_str(), rhs->get_name()->c_str()) < 0;
    }
  };

  size_t home_node; // The id of the node this keyvalue store is running on
  size_t num_nodes;

  std::map<Key *, Lock *, KeyComp> waiting_listeners;
  Lock listener_lock;

  std::map<Key *, DistributedValue *, KeyComp> kv_map;
  std::map<Key *, DataFrame *, KeyComp> distro_kv_map;
  Lock kv_lock;
  Lock distro_kv_lock;

  LocalNetworkMessageManager *local_network_layer;
  SocketNetworkMessageManager *real_network_layer;

  /**
   * Function to signal that whoever is waiting for this key that this key
   * now is ready and has a value.
   * @param key
   */
  void signal_listener(Key &key);

  /**
   * Function to wait until the provided key has been added to the key-value
   * store.
   * @param key
   */
  void wait_for_listener(Key &key);
};

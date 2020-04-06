/**
 * Name: Snowy Chen, Joe Song
 * Date: 21 March 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::Cpp

#pragma once

#include <map>
#include "dataframe.h"
#include "distributed_app.h"
#include "key.h"
#include "thread.h"

class Application;
class LocalNetworkMessageManager;
class RealNetworkMessageManager;
class StatusHandler;

class KeyValueStore : public CustomObject {
public:
  explicit KeyValueStore(size_t num_nodes);
  ~KeyValueStore() override;

  /**
   * Puts the dataframe to the associated key. If the key-value store is not the
   * one that is supposed to store this dataframe, it will give the dataframe to
   * the correct home node. Once the value has been passed to the function,
   * the KV-store will now own the value.
   * @param key The key that this value will be associated with
   * @param value The value that will be stored. All dataframes put in the
   * kv-store will be owned by the kvstore
   */
  void put(Key &key, DataFrame *value);

  /**
   * Wait and gets the associated value of the key.
   * @param key The key that the desired value is associated with.
   * @return The dataframe value associated with the key. The dataframe will be
   * dynamically allocated the caller will now own the dataframe. This can be
   * called for locally stored dataframes.
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
   * Map through the dataframe stored at the specified key. This is only
   * valid for keys that are local to the application. If the dataframe is
   * not local, then it will not do anything.
   * @param key The key that the dataframe is associated with
   * @param rower The rower to row through the dataframe.
   */
  void local_map(Key &key, Rower &rower);

  /**
   * Iterates through all of the local dataframes within the key-store. Call
   * "start_iter" in order to start the iteration. next_iter() to move to the
   * next. has_next() to see if there is another key value pair that is
   * available locally. get_iter_key() to get the key of the current iterator
   * get_iter_value() to get the value of the current local key-value store.
   */
  void start_iter();
  bool has_next();
  void next_iter();
  Key *get_iter_key();
  DataFrame *get_iter_value();

  void send_status_message(size_t node_id, String &msg);

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
  LocalNetworkMessageManager *connect_local(size_t node_id,
      StatusHandler* status_handler);

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
  RealNetworkMessageManager *connect_network(StatusHandler* status_handler);

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
  static void from_array(Key &key, KeyValueStore *kv, size_t num_values,
                               float *values);
  static void from_array(Key &key, KeyValueStore *kv, size_t num_values,
                               int *values);
  static void from_array(Key &key, KeyValueStore *kv, size_t num_values,
                               bool *values);
  static void from_array(Key &key, KeyValueStore *kv, size_t num_values,
                               String **values);

  /**
   * Generates a Dataframe value, and then stores it within the map at the
   * specified key.
   * @param key The key associated with the new dataframe.
   * @param map The map that will store the dataframe at the specified key.
   * @param value The value that the dataframe will store.
   */
  static void from_scalar(Key &key, KeyValueStore *kv, bool value);
  static void from_scalar(Key &key, KeyValueStore *kv, int value);
  static void from_scalar(Key &key, KeyValueStore *kv, float value);
  static void from_scalar(Key &key, KeyValueStore *kv, String *value);

  /**
   * Generates a dataframe using a visitor and storing it with the key at the
   * key-value store.
   * @param key The key to store the generated dataframe with.
   * @param kv The key-value store to store the dataframe
   * @param schema_types The schema type of the dataframe
   * @param writer The writer visitor that will build the dataframe
   */
  static void from_visitor(Key &key, KeyValueStore *kv,
      const char *schema_types, Writer &writer);

  /**
   * Gets the home node id of this KV store ONLY if the distributed
   * application layer has already been configured.
   * @return The home node id.
   * @throws Throws an error and terminates the program if the distributed
   * application layer has not been properly configured.
   */
  size_t get_home_id();

  size_t get_num_nodes() { return this->num_nodes; }

private:
  class KeyComp {
  public:
    bool operator() (const Key *lhs, const Key *rhs) const {
      // Ensure that all of the map keys have keys that live in the same home id
      assert(lhs->get_home_id() == rhs->get_home_id());

      return strcmp(lhs->get_name()->c_str(), rhs->get_name()->c_str()) < 0;
    }
  };

  static const size_t MAX_NUM_DISTRIBUTED_ROWS = 50;

  std::map<Key *, DataFrame *, KeyComp> kv_map;
  std::map<Key *, DataFrame *, KeyComp>::iterator iter = kv_map.end();
  size_t home_node; // The id of the node this keyvalue store is running on
  size_t num_nodes;
  Lock kv_lock;

  LocalNetworkMessageManager *local_network_layer;
  RealNetworkMessageManager *real_network_layer;

  /**
   * Generates a dataframe that is distributed into dataframe segments into
   * the provided key-value store. It will auto-generate the keys to
   * associate with each segment of the dataframe by using the key_prefix. It
   * will also distribute the keys fairly across all of the nodes within the
   * key-value store.
   * @param key_prefix The prefix of the key name that will be used to
   * autogenerate the keys.
   * @param kv The key-value store to store the values in.
   * @param schema_types The schema type of the dataframe.
   * @param writer The writer vistor that will build the dataframe.
   * @param max_num_rows The maximum number of rows that a segment of the
   * dataframe will contain.
   */
  static void from_visitor_distributed(const char *key_prefix,
      KeyValueStore *kv, const char *schema_types, Writer &writer,
      size_t max_num_rows);
};

/**
 * Name: Snowy Chen, Joe Song
 * Date: 21 March 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::Cpp

#pragma once

#include "dataframe.h"
#include "key.h"
#include "map.h"

class KeyValueStore : public CustomObject {
public:
  explicit KeyValueStore(size_t home_node);

  /**
   * Puts the dataframe to the associated key. If the key-value store is not the
   * one that is supposed to store this dataframe, it will give it to the
   * correct home node.
   * @param key The key that this value will be associated with
   * @param value The value that will be stored
   */
  void put(Key &key, DataFrame *value);

  /**
   * Wait and gets the associated value of the key.
   * @param key The key that the desired value is associated with.
   * @return The dataframe value associated with the key. The dataframe will be
   * dynamically allocated the caller will now own the dataframe.
   */
  DataFrame *wait_and_get(Key &key);

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

private:
  Map kv;
  size_t home_node; // The id of the node this keyvalue store is running on
};

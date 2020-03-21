/**
 * Name: Snowy Chen, Joe Song
 * Date: 21 February 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::CwC

#pragma once

#include "dataframe.h"
#include "map.h"

class KeyValueStore : public CustomObject {
public:
  Map kv;
  size_t home_node; // The id of the node this keyvalue store is running on

  KeyValueStore(size_t home_node) { this->home_node = home_node; }

  /**
   * Puts the dataframe to the associated key. If the key-value store is not the
   * one that is supposed to store this dataframe, it will give it to the
   * correct home node.
   * @param key The key that this value will be associated with
   * @param value The value that will be stored
   */
  void put(Key *key, DataFrame *value) {
    assert(key != nullptr);
    assert(ret_vavaluelue != nullptr);

    if (key->home_id == this->home_node) {
      this->kv.put(key, value);
    } else {
      // TODO: Link the key value stores together so they can talk to each
      // other.
    }
  }

  /**
   * Wait and gets the associated value of the key.
   * @param key The key that the desired value is associated with.
   * @return The dataframe value associated with the key. The dataframe will be
   * dynamically allocated the caller will now own the dataframe.
   */
  DataFrame *waitAndGet(Key &key) {
    if (key.home_id == this->home_node) {
      DataFrame *value = reinterpret_cast<DataFrame *>(this->kv.get(&key));

      // Now copy the value so it can be owned by the caller.
      DataFrame *ret_value = new DataFrame(*value);
      // TODO: Create a rower/fielder to copy the dataframe into a new
      // dataframe.

      return ret_value;
    } else {
      // TODO: Link the key value stores together so they can talk to each
      // other.
    }
  }

  /**
   * Generates a Dataframe out of the provided array, and then stores it
   * within the map at the specified key.
   * @param key The key associated with the new dataframe.
   * @param kv The map that will store the dataframe at the specified key.
   * @param num_values The number of values of the provided array.
   * @param values The array of valuas that will be used to create a dataframe
   * @return The newly created dataframe
   */
  static DataFrame *fromArray(Key *key, KeyValueStore *kv, size_t num_values,
                              float *values) {
    assert(key != nullptr);
    assert(kv != nullptr);
    assert(values != nullptr);

    // Create the dataframe
    Schema schema("F");
    DataFrame *ret_value = new DataFrame(schema);

    for (size_t i = 0; i < num_values; i++) {
      Row r(ret_value->get_schema());
      r.set(0, values[i]);
      ret_value->add_row(r);
    }

    kv->put(key, ret_value);

    return ret_value;
  }

  static DataFrame *fromArray(Key *key, KeyValueStore *kv, size_t num_values,
                              int *values) {
    assert(key != nullptr);
    assert(kv != nullptr);
    assert(values != nullptr);

    // Create the dataframe
    Schema schema("I");
    DataFrame *ret_value = new DataFrame(schema);

    for (size_t i = 0; i < num_values; i++) {
      Row r(ret_value->get_schema());
      r.set(0, values[i]);
      ret_value->add_row(r);
    }

    kv->put(key, ret_value);

    return ret_value;
  }

  static DataFrame *fromArray(Key *key, KeyValueStore *kv, size_t num_values,
                              bool *values) {
    assert(key != nullptr);
    assert(kv != nullptr);
    assert(values != nullptr);

    // Create the dataframe
    Schema schema("B");
    DataFrame *ret_value = new DataFrame(schema);

    for (size_t i = 0; i < num_values; i++) {
      Row r(ret_value->get_schema());
      r.set(0, values[i]);
      ret_value->add_row(r);
    }

    kv->put(key, ret_value);

    return ret_value;
  }

  static DataFrame *fromArray(Key *key, KeyValueStore *kv, size_t num_values,
                              String **values) {
    assert(key != nullptr);
    assert(kv != nullptr);
    assert(values != nullptr);

    // Create the dataframe
    Schema schema("S");
    DataFrame *ret_value = new DataFrame(schema);

    for (size_t i = 0; i < num_values; i++) {
      Row r(ret_value->get_schema());
      r.set(0, values[i]);
      ret_value->add_row(r);
    }

    kv->put(key, ret_value);

    return ret_value;
  }

  /**
   * Generates a Dataframe value, and then stores it within the map at the
   * specified key.
   * @param key The key associated with the new dataframe.
   * @param map The map that will store the dataframe at the specified key.
   * @param value The value that the dataframe will store.
   * @return The newly created dataframe
   */
  static DataFrame *fromScalar(Key *key, KeyValueStore *kv, bool value) {
    assert(key != nullptr);
    assert(kv != nullptr);

    // Create the dataframe
    Schema schema("B");
    DataFrame *ret_value = new DataFrame(schema);

    // Store the value
    Row r(ret_value->get_schema());
    r.set(0, value);
    ret_value->add_row(r);

    kv->put(key, ret_value);

    return ret_value;
  }

  static DataFrame *fromScalar(Key *key, KeyValueStore *kv, int value) {
    assert(key != nullptr);
    assert(kv != nullptr);

    // Create the dataframe
    Schema schema("I");
    DataFrame *ret_value = new DataFrame(schema);

    // Store the value
    Row r(ret_value->get_schema());
    r.set(0, value);
    ret_value->add_row(r);

    kv->put(key, ret_value);

    return ret_value;
  }

  static DataFrame *fromScalar(Key *key, KeyValueStore *kv, float value) {
    assert(key != nullptr);
    assert(kv != nullptr);

    // Create the dataframe
    Schema schema("F");
    DataFrame *ret_value = new DataFrame(schema);

    // Store the value
    Row r(ret_value->get_schema());
    r.set(0, value);
    ret_value->add_row(r);

    kv->put(key, ret_value);

    return ret_value;
  }

  static DataFrame *fromScalar(Key *key, KeyValueStore *kv, String *value) {
    assert(key != nullptr);
    assert(kv != nullptr);

    // Create the dataframe
    Schema schema("S");
    DataFrame *ret_value = new DataFrame(schema);

    // Store the value
    Row r(ret_value->get_schema());
    r.set(0, value);
    ret_value->add_row(r);

    kv->put(key, ret_value);

    return ret_value;
  }
};

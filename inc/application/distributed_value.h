/**
 * Name: Snowy Chen, Joe Song
 * Date: 8 April 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::Cpp

#pragma once

#include "custom_object.h"
#include "custom_string.h"
#include "key.h"

class KeyValueStore;
class DataFrame;
class Schema;
class Row;
class Rower;

/**
 * A value of a Key-value store within a eau2 system. This should be used
 * exclusively with a key-value store running on the application that is
 * running on a single node.
 */
class DistributedValue : public CustomObject {
public:
  DistributedValue(Key &key, Schema &schema, KeyValueStore *kv);
  ~DistributedValue() override;

  Schema &get_schema();

  // These functions are used to add values to the distributed value. Once
  // package_value() is called, it will then send the value throughout the
  // key-value store this value has been passed in with, then that means that
  // the distributed value has become final, and should not be modified
  // anymore. If it is modified after being packaged, the behavior is undefined
  void add_row(Row &row);
  void package_value(); // Should not be called more than once.

  /** Return the value at the given column and row. Accessing rows or
   *  columns out of bounds, or request the wrong type is undefined.*/
  int get_int(size_t col, size_t row);
  bool get_bool(size_t col, size_t row);
  float get_float(size_t col, size_t row);
  String *get_string(size_t col, size_t row);

  /** The number of rows in the dataframe. */
  size_t nrows();

  /** The number of columns in the dataframe.*/
  size_t ncols();

  /** Visit rows in order that are locally stored for this application node */
  void local_map(Rower &r);
  void map(Rower &r);

  void serialize(Serializer &serializer) override;
  size_t serialization_required_bytes() override;
  static DistributedValue *deserialize_as_distributed_val(
      Deserializer &deserializer, Key &key, KeyValueStore *kv);

private:
  static const size_t MAX_NUM_ROWS = 50;

  Key *key;
  Schema *schema;
  KeyValueStore *kv;

  size_t num_rows;

  Key *cached_key;
  DataFrame *cached_df;
  void cache_df(size_t row);

  // These are some status fields that are used to build the distributed
  // value. It's automatically handled by the add_row() and package_value()
  // functions
  size_t writer_node_id;
  size_t writer_curr_num_segments;
  size_t writer_curr_row_num;
  DataFrame *building_df;

  friend class Writer;
};
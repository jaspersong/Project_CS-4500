#pragma once

#include "custom_object.h"

class Map : public CustomObject {
public:
  static const size_t DEFAULT_HASH_SIZE = 64;

  Map();
  explicit Map(size_t hash_table_size);
  ~Map() override;

  // Does this map contain the given key
  bool contains_key(CustomObject *key);

  // Places the given value at the given key
  void put(CustomObject *key, CustomObject *value);

  // Returns the value at the given key
  CustomObject *get(CustomObject *key);

  // Does this map equal the given Object
  bool equals(CustomObject *other) override;

  // Computes the hash value of this map
  size_t hash_me() override;

  // Returns the set of keys in this map
  CustomObject **key_set();

  // Removes and returns the object at the given key
  CustomObject *remove(CustomObject *key);

  // Returns the number of elements in this map
  int size() { return static_cast<int>(this->num_elements); }

private:
  class HashChainNode;

  HashChainNode **hash_table;
  size_t hash_table_size;
  size_t num_elements;
};
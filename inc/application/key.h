/**
 * Name: Snowy Chen, Joe Song
 * Date: 21 March 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::Cpp

#pragma once

#include "custom_object.h"
#include "custom_string.h"

/**
 * A key of a Key-value store within a eau2 system.
 */
class Key : public CustomObject {
public:
  // Create a Key associated with a whole distributed value
  explicit Key(const char *name);

  // Create a key associated with a chunk of a distributed value
  Key(const char *name, size_t home_id);

  ~Key() override;

  size_t hash_me() override;
  bool equals(CustomObject *other) const override;
  void serialize(Serializer &serializer) override;
  size_t serialization_required_bytes() override;

  String *get_name() const { return this->name; }

  // Gets the home id of the value. If it's -1, then it's associated with a
  // distributed value. If it's any other number, than it's associated with a
  // chunk of a distributed value
  size_t get_home_id() const { return this->home_id; }

  /**
   * Interprets a serialized piece of data as a key, and returns the value
   * as a key.
   * @param deserializer The deserializer containing the serialized data we
   *        are trying to deserialize.
   * @return Key pulled from the serialized data buffer. Nullptr if it
   *            failed to retrieve a string from the deserializer
   */
  static Key *deserialize_as_key(Deserializer &deserializer);

private:
  String *name; // owned
  size_t home_id;
};
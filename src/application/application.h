/**
 * Name: Snowy Chen, Joe Song
 * Date: 21 February 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::CwC

#pragma once

#include "custom_object.h"
#include "key_value_store.h"
#include "map.h"
#include "thread.h"

/**
 * An interface that details the API functions needed for an application that
 * will be run on the server of the eau2 system.
 */
class Application : public Thread {
public:
  KeyValueStore *kv;

  Application(size_t node_id) { this->kv = new KeyValueStore(node_id); }

  /**
   * The main function of the application.
   */
  void run() override { assert(false); }

  /**
   * Gets the node id that the application is running on.
   * @return The node id that the application is running on. -1 if it is running
   * on the server.
   */
  size_t get_node_id() { return -1; }
};

/**
 * A key of a Key-value store within a eau2 system.
 */
class Key : public CustomObject {
public:
  String *name; // owned
  size_t home_id;

  Key(const char *name, size_t home_id) {
    this->name = new String(name);
    this->home_id = home_id;
  }

  ~Key() override { delete this->name; }

  size_t hash_me() override { return this->home_id + this->name->hash(); };

  bool equals(CustomObject *other) override {
    if (other == this)
      return true;
    Key *x = dynamic_cast<Key *>(other);
    if (x == nullptr)
      return false;
    return (this->home_id == x->home_id) && this->name->equals(x->name);
  }

  void serialize(Serializer &serializer) override {
    serializer.set_size_t(this->home_id);
    this->name->serialize(serializer);
  }

  size_t serialization_required_bytes() override {
    return Serializer::get_required_bytes(this->home_id) +
           this->name->serialization_required_bytes();
  }
};

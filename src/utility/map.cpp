/**
 * Name: Snowy Chen, Joe Song
 * Date: 22 March 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::Cpp

#include "map.h"

// A private class that is a linked list node stemming from the Map's hash
// table chain.
class Map::HashChainNode {
public:
  HashChainNode *next_{};
  CustomObject *key_;
  CustomObject *value_;

  // Constructs a hash chain node with the provided information
  HashChainNode(CustomObject *key, CustomObject *value) {
    this->key_ = key;
    this->value_ = value;
  }
};

Map::Map() : Map(Map::DEFAULT_HASH_SIZE) {}

Map::Map(size_t hash_table_size) {
  this->hash_table_size = hash_table_size;
  this->hash_table = new HashChainNode *[hash_table_size];

  // Initialize the hash table
  for (size_t i = 0; i < this->hash_table_size; i++) {
    this->hash_table[i] = nullptr;
  }
  this->num_elements = 0;
}

Map::~Map() {
  // Iterate through the hash table buckets and delete the linked lists
  // node by node
  for (size_t i = 0; i < this->hash_table_size; i++) {
    HashChainNode *curr_node = this->hash_table[i];
    HashChainNode *next_node = nullptr;
    while (curr_node != nullptr) {
      // Save the next node in order for us to be able to move on
      next_node = curr_node->next_;

      // Free the node
      delete curr_node;

      // Move onto the next one
      curr_node = next_node;
    }
  }

  // Now delete the allocated hash table buckets.
  delete[] this->hash_table;
}

bool Map::contains_key(CustomObject *key) {
  // Get the hash of the key in order to determine which bucket we need to
  // look into in the hash table.
  size_t key_hash = key->hash();

  // Get the linked list hash chain and iterate through it in order to find
  // the key itself
  HashChainNode *curr_node =
      this->hash_table[key_hash % this->hash_table_size];
  while (curr_node != nullptr) {
    // Take the node and see if it contains the key
    if (curr_node->key_->equals(key)) {
      // Found it!
      return true;
    }

    // Move onto the next node of the linked list
    curr_node = curr_node->next_;
  }

  // Could not find it in there. Return false
  return false;
}

void Map::put(CustomObject *key, CustomObject *value) {
  // Get the hash of the key in order to determine which bucket we need to
  // look into in the hash table.
  size_t key_hash = key->hash();

  // Get the linked list hash chain and iterate through it in order to see
  // if this key has already been filled.
  HashChainNode *curr_node =
      this->hash_table[key_hash % this->hash_table_size];
  while (curr_node != nullptr) {
    // Take the node and see if it contains the key
    if (curr_node->key_->equals(key)) {
      // Found the key already set. Update the value.
      curr_node->value_ = value;
      return;
    }

    // Move onto the next node of the linked list
    curr_node = curr_node->next_;
  }

  // Did not find the key already in there. Simply tack it onto the head of
  // the linked list chain
  HashChainNode *curr_head =
      this->hash_table[key_hash % this->hash_table_size];
  auto *new_head = new HashChainNode(key, value);
  new_head->next_ = curr_head;
  this->hash_table[key_hash % this->hash_table_size] = new_head;

  // Increment the number of elements accordingly
  this->num_elements += 1;
}

CustomObject *Map::get(CustomObject *key) {
  // Get the hash of the key in order to determine which bucket we need to
  // look into in the hash table.
  size_t key_hash = key->hash();

  // Get the linked list hash chain and iterate through it in order to see
  // if this key has already been filled.
  HashChainNode *curr_node =
      this->hash_table[key_hash % this->hash_table_size];
  while (curr_node != nullptr) {
    // Take the node and see if it contains the key
    if (curr_node->key_->equals(key)) {
      // Found the key. Now return with the value
      return curr_node->value_;
    }

    // Move onto the next node of the linked list
    curr_node = curr_node->next_;
  }

  // Could not find the key. There is no value. Return nullptr
  return nullptr;
}

bool Map::equals(CustomObject *other) {
  if (other == nullptr) {
    return false;
  }

  Map *m = dynamic_cast<Map *>(other);

  if (m == nullptr) {
    return false;
  } else if (this->hash_table_size == m->hash_table_size) {
    // Okay, now we iterate through all the hash chains and compare the
    // contents to see if everything is equal
    for (size_t i = 0; i < this->hash_table_size; i++) {
      HashChainNode *this_curr_node = this->hash_table[i];
      HashChainNode *that_curr_node = m->hash_table[i];

      while (this_curr_node != nullptr && that_curr_node != nullptr) {
        // Now compare nodes!
        if (!this_curr_node->key_->equals(that_curr_node->key_) ||
            !this_curr_node->value_->equals(that_curr_node->value_)) {
          // Found a node with differing content. Return false
          return false;
        }

        // Move onto the next nodes
        this_curr_node = this_curr_node->next_;
        that_curr_node = that_curr_node->next_;
      }
    }

    // Found no differences.
    return true;
  } else {
    // Not the same hash table size.
    return false;
  }
}

size_t Map::hash_me() {
  size_t ret_value = this->hash_table_size;

  // Iterate through all the keys and values and add them to the hash
  for (size_t i = 0; i < this->hash_table_size; i++) {
    HashChainNode *curr_node = this->hash_table[i];

    while (curr_node != nullptr) {
      // Add the value
      ret_value += curr_node->key_->hash();
      ret_value += curr_node->value_->hash();

      // Move onto the next nodes
      curr_node = curr_node->next_;
    }
  }

  return ret_value;
}

CustomObject **Map::key_set() {
  // Allocate the array containing the sets of keys
  auto **ret_value = new CustomObject *[this->size()];

  // Now iterate through the map and its chains and slowly add them to the
  // array
  size_t ret_index = 0;
  for (size_t i = 0; i < this->hash_table_size; i++) {
    HashChainNode *curr_node = this->hash_table[i];
    while (curr_node != nullptr) {
      // Add the key to the return value
      ret_value[ret_index] = curr_node->key_;
      ret_index += 1;

      // Move onto the next node
      curr_node = curr_node->next_;
    }
  }

  return ret_value;
}

CustomObject *Map::remove(CustomObject *key) {
  // Get the hash of the key in order to determine which bucket we need to
  // look into in the hash table.
  size_t key_hash = key->hash();

  // Get the linked list hash chain and iterate through it in order to find
  // the provided key
  HashChainNode *curr_node =
      this->hash_table[key_hash % this->hash_table_size];
  HashChainNode *prev_node = nullptr;
  while (curr_node != nullptr) {
    // Take the node and see if it contains the key
    if (curr_node->key_->equals(key)) {
      // Found the key. Now remove it from the list and return the value.
      if (prev_node != nullptr) {
        prev_node->next_ = curr_node->next_;
      } else {
        // This node is the head of the chain list. Update the head
        // accordingly.
        this->hash_table[key_hash % this->hash_table_size] = curr_node->next_;
      }

      // Decrement the size accordingly
      this->num_elements -= 1;

      // Now get the return value, free the hash chain as well.
      CustomObject *ret_value = curr_node->value_;
      delete curr_node;
      return ret_value;
    }

    // Move onto the next node of the linked list
    prev_node = curr_node;
    curr_node = curr_node->next_;
  }

  // Could not find the key. There is no value. Return nullptr
  return nullptr;
}

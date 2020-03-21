/**
 * Name: Snowy Chen, Joe Song
 * Date: 14 February 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// lang::CwC

#pragma once

#include <math.h>
#include <stdarg.h>
#include <stdlib.h>

#include "custom_object.h"
#include "custom_string.h"

/**
 * A union of all the different column data types. The determination of which
 * data type that the item actually is determined by the column's
 * ColumnType_t, if this union is used within the Column and its derivation
 * classes. This is used so the Column class that all other specific
 * columns inherit from can reference a single implementation of the linked list
 * of arrays without having to duplicate code.
 *
 * If this union is used by any other classes (i.e., Schema and Row), they must
 * remember the data type on its own.
 */
typedef union {
  bool b;
  int i;
  float f;
  String *s;

  // This value is a generalization in order for other classes to be able to
  // store other data types into this data item. This value is primarily used
  // by Schema and Row, whereas the primitives mentioned above is to cater to
  // Column.
  CustomObject *o;
} DataItem_;

/**
 * A private class that holds a single chain of data items within the
 * ArrayOfArrays class.
 */
class ArrayNode_ {
public:
  size_t array_size;
  DataItem_ *array;

  /**
   * Constructs the array node.
   * @param array_size The number of items that are within this array node.
   */
  explicit ArrayNode_(size_t array_size) {
    this->array_size = array_size;
    this->array = new DataItem_[this->array_size];
  }

  /**
   * Deconstructs the array node.
   */
  ~ArrayNode_() {
    delete[] this->array;
    this->array = nullptr;
    this->array_size = 0;
  }
};

/**
 * A resizeable data structure with a O(1) getting methods. This class stores
 * items in a DataItem_, which is a union of various other primitives. The
 * data type of the value stored within the DataItem must be determined by
 * the class that uses ArrayOfArrays.
 *
 * It contains an array of arrays of DataItem. In the first layer of the
 * array, it is made up of array nodes, which have a quadratic growth
 * starting from 2 in size.
 */
class ArrayOfArrays : public CustomObject {
public:
  ArrayNode_ **nodes_;
  size_t num_nodes_;
  size_t offset_;

  /**
   * Constructs an array of arrays.
   */
  ArrayOfArrays() {
    this->nodes_ = new ArrayNode_ *[1];
    this->nodes_[0] = new ArrayNode_(2);
    this->num_nodes_ = 1;
    this->offset_ = 0;
  }

  /**
   * A copy constructor that copies the provided array of arrays into another
   * instance of that array of arrays.
   * @param list The original list to copy from.
   */
  ArrayOfArrays(ArrayOfArrays &list) : ArrayOfArrays() {
    // Iterate through the provided list in order to copy their data.
    for (size_t index = 0; index < list.size(); index++) {
      DataItem_ item = list.get_item(index);
      this->add_new_item(item);
    }
  }

  /**
   * Deconstructs an array of arrays.
   */
  ~ArrayOfArrays() override {
    for (size_t i = 0; i < this->num_nodes_; i++) {
      delete this->nodes_[i];
    }
    delete[] this->nodes_;
  }

  /**
   * Private helper function that determines the node index and the offset
   * within that node in regards to the provided index.
   * @param index The index of the desired item.
   */
  static void get_node_index_offset(size_t index, size_t &node_index,
                                    size_t &node_offset) {
    size_t num_items_thus_far = 0;
    node_index = 0;

    size_t next_size = 1 << (node_index + 1);
    while (num_items_thus_far + next_size < index + 1) {
      node_index += 1;
      num_items_thus_far += next_size;
      next_size = 1 << (node_index + 1);
    }

    node_offset = index - num_items_thus_far;
  }

  /**
   * Grows the array of arrays so it has the specified number of nodes.
   * @param num_nodes Number nodes that this array of arrays should grow to have
   */
  void grow_(size_t new_num_nodes) {
    // Do nothing if the specified number of nodes is not greater than the
    // number of nodes we have right now
    if (this->num_nodes_ < new_num_nodes) {
      ArrayNode_ **new_array_ = new ArrayNode_ *[new_num_nodes];

      // Copy the nodes from the previous array, and add new blank nodes for
      // the newly created space in the first layer of the array
      for (size_t i = 0; i < new_num_nodes; i++) {
        if (i < this->num_nodes_) {
          new_array_[i] = this->nodes_[i];
        } else {
          new_array_[i] = new ArrayNode_(pow(2, i + 1));
        }
      }

      // Now replace the arrays
      this->num_nodes_ = new_num_nodes;
      delete[] this->nodes_;
      this->nodes_ = new_array_;
    }
  }

  /**
   * A helper function that adds a new item to the array of array.
   * @param item The new Data item that will be added.
   */
  void add_new_item(DataItem_ item) {
    // Calculate the node index and the node offset
    size_t node_index;
    size_t node_offset;
    ArrayOfArrays::get_node_index_offset(this->offset_, node_index,
                                         node_offset);

    // Grow the array if necessary
    this->grow_(node_index + 1);

    // Store the item at the offset index
    this->nodes_[node_index]->array[node_offset] = item;

    // Increment the offset
    this->offset_ += 1;
  }

  /**
   * A helper function that sets the item at the specified 0-indexed index
   * within the linked list of array of items. This function should not be
   * called by classes that does not inherit from Column.
   * @param index The 0-index of the item that this new item should replace
   * @param item The item that should be set to.
   * @throws If the index is invalid, it will print a message to stdout
   *         and terminate the program
   */
  void set_new_item(size_t index, DataItem_ item) {
    // Check to make sure that the item is within bounds
    if ((index >= this->offset_) || (index < 0)) {
      printf("Index %zu is out of bounds.\n", index);
      exit(1);
    }

    // Calculate the node index and the node offset
    size_t node_index;
    size_t node_offset;
    ArrayOfArrays::get_node_index_offset(index, node_index, node_offset);

    // Store the item at the offset index
    this->nodes_[node_index]->array[node_offset] = item;
  }

  /**
   * Helper function that gets the item at the specified 0-indexed index
   * within the linked list of array of items. This function should not be
   * called by classes that does not inherit from Column.
   * @param index The 0-index of the desired item
   * @return A DataItem_ representation of the item. The interpretation of
   *         the actual data should be determined by the column data type.
   * @throws If the index is invalid, it will print a message to stdout and
   *         terminate the program
   */
  DataItem_ get_item(size_t index) {
    // Check to make sure that the item is within bounds
    if ((index >= this->offset_) || (index < 0)) {
      printf("Index %zu is out of bounds for this column.\n", index);
      exit(1);
    }

    // Calculate the node index and the node offset
    size_t node_index;
    size_t node_offset;
    ArrayOfArrays::get_node_index_offset(index, node_index, node_offset);

    // Get the value
    return this->nodes_[node_index]->array[node_offset];
  }

  /**
   * Gets the number of items within this data structure.
   * @return The number of items within this data structure.
   */
  size_t size() { return this->offset_; }
};

/**
 * Name: Snowy Chen, Joe Song
 * Date: 21 March 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::Cpp

#include <cmath>
#include "dataframe_helper.h"

/**
 * A private class that holds a single chain of data items within the
 * ArrayOfArrays class.
 */
class ArrayOfArrays::ArrayNode {
public:
  size_t array_size;
  DataItem_ *array;

  /**
   * Constructs the array node.
   * @param array_size The number of items that are within this array node.
   */
  explicit ArrayNode(size_t array_size) {
    this->array_size = array_size;
    this->array = new DataItem_[this->array_size];
  }

  /**
   * Deconstructs the array node.
   */
  ~ArrayNode() {
    delete[] this->array;
    this->array = nullptr;
    this->array_size = 0;
  }
};

ArrayOfArrays::ArrayOfArrays() {
  this->nodes = new ArrayNode *[1];
  this->nodes[0] = new ArrayNode(2);
  this->num_nodes = 1;
  this->offset = 0;
}

ArrayOfArrays::ArrayOfArrays(ArrayOfArrays &list) : ArrayOfArrays() {
  // Iterate through the provided list in order to copy their data.
  for (size_t index = 0; index < list.size(); index++) {
    DataItem_ item = list.get_item(index);
    this->add_new_item(item);
  }
}

ArrayOfArrays::~ArrayOfArrays() {
  for (size_t i = 0; i < this->num_nodes; i++) {
    delete this->nodes[i];
  }
  delete[] this->nodes;
}

void ArrayOfArrays::get_node_index_offset(size_t index, size_t &node_index,
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

void ArrayOfArrays::grow(size_t new_num_nodes) {
  // Do nothing if the specified number of nodes is not greater than the
  // number of nodes we have right now
  if (this->num_nodes < new_num_nodes) {
    auto **new_array_ = new ArrayNode *[new_num_nodes];

    // Copy the nodes from the previous array, and add new blank nodes for
    // the newly created space in the first layer of the array
    for (size_t i = 0; i < new_num_nodes; i++) {
      if (i < this->num_nodes) {
        new_array_[i] = this->nodes[i];
      } else {
        new_array_[i] = new ArrayNode(pow(2, i + 1));
      }
    }

    // Now replace the arrays
    this->num_nodes = new_num_nodes;
    delete[] this->nodes;
    this->nodes = new_array_;
  }
}

void ArrayOfArrays::add_new_item(DataItem_ item) {
  // Calculate the node index and the node offset
  size_t node_index;
  size_t node_offset;
  ArrayOfArrays::get_node_index_offset(this->offset, node_index, node_offset);

  // Grow the array if necessary
  this->grow(node_index + 1);

  // Store the item at the offset index
  this->nodes[node_index]->array[node_offset] = item;

  // Increment the offset
  this->offset += 1;
}

void ArrayOfArrays::set_new_item(size_t index, DataItem_ item) {
  // Check to make sure that the item is within bounds
  if ((index >= this->offset) || (index < 0)) {
    printf("Index %zu is out of bounds.\n", index);
    assert(false);
  }

  // Calculate the node index and the node offset
  size_t node_index;
  size_t node_offset;
  ArrayOfArrays::get_node_index_offset(index, node_index, node_offset);

  // Store the item at the offset index
  this->nodes[node_index]->array[node_offset] = item;
}

DataItem_ ArrayOfArrays::get_item(size_t index) {
  // Check to make sure that the item is within bounds
  if ((index >= this->offset) || (index < 0)) {
    printf("Index %zu is out of bounds for this column.\n", index);
    assert(false);
  }

  // Calculate the node index and the node offset
  size_t node_index;
  size_t node_offset;
  ArrayOfArrays::get_node_index_offset(index, node_index, node_offset);

  // Get the value
  return this->nodes[node_index]->array[node_offset];
}

size_t ArrayOfArrays::size() { return this->offset; }

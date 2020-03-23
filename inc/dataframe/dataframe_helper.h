/**
 * Name: Snowy Chen, Joe Song
 * Date: 14 February 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// lang::CwC

#pragma once

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
  ArrayOfArrays();

  /**
   * A copy constructor that copies the provided array of arrays into another
   * instance of that array of arrays.
   * @param list The original list to copy from.
   */
  ArrayOfArrays(ArrayOfArrays &list);

  ~ArrayOfArrays() override;

  /**
   * Private helper function that determines the node index and the offset
   * within that node in regards to the provided index.
   * @param index The index of the desired item.
   */
  static void get_node_index_offset(size_t index, size_t &node_index,
                                    size_t &node_offset);

  /**
   * A helper function that adds a new item to the array of array.
   * @param item The new Data item that will be added.
   */
  void add_new_item(DataItem_ item);

  /**
   * A helper function that sets the item at the specified 0-indexed index
   * within the linked list of array of items. This function should not be
   * called by classes that does not inherit from Column.
   * @param index The 0-index of the item that this new item should replace
   * @param item The item that should be set to.
   * @throws If the index is invalid, it will print a message to stdout
   *         and terminate the program
   */
  void set_new_item(size_t index, DataItem_ item);

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
  DataItem_ get_item(size_t index);

  /**
   * Gets the number of items within this data structure.
   * @return The number of items within this data structure.
   */
  size_t size();

private:
  class ArrayNode;

  ArrayNode **nodes;
  size_t num_nodes;
  size_t offset;

  /**
   * Grows the array of arrays so it has the specified number of nodes.
   * @param num_nodes Number nodes that this array of arrays should grow to have
   */
  void grow(size_t new_num_nodes);
};

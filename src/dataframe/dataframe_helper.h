/**
 * Name: Snowy Chen, Joe Song
 * Date: 14 February 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

//lang::CwC

#pragma once

#include <stdarg.h>
#include <stdlib.h>

#include "object.h"
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
 * remember the data type on its own. This is okay because Row and Schema
 * only ever store strings.
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
  Object *o;
} DataItem_;

/**
 * A private class that represents a singular node within a linked list used
 * by Column. Each node contains an ARRAY_SIZE number of DataItem_. This
 * linked list of arrays will be controlled by functions within Column, that
 * the inherited column classes can use in order to store their data.
 */
class LinkedListArrayNode_ {
public:
  // The size of the array that the array node contains
  static const size_t ARRAY_SIZE = 4096;

  LinkedListArrayNode_ *next_ = nullptr; // Pointer to the next node.
  DataItem_ array[ARRAY_SIZE] = {false}; // The array that this node holds.
};

/**
 * A helper class for Column, Row and Schema in order to have a resizeable
 * data structure with sort of O(1) algorithmic getting methods. This class
 * primarily stores items in DataItem_, which is a union of various other
 * primitives. The data type of the value stored within a DataItem must be
 * determined by the class that uses the LinkedListArray_.
 *
 * NOTE: This linked list of arrays does NOT support removing items. Only
 * replacing them with different values, or adding new ones. This is because
 * Row, Schema and Column APIs do not have a need for removing items.
 */
class LinkedListArray_ : public Object {
public:
  // Linked list of array of items within the column.
  LinkedListArrayNode_ *list_head_;
  LinkedListArrayNode_ *list_tail_;
  size_t size_;

  /**
   * Constructs a linked list of array of data items.
   */
  LinkedListArray_() {
    this->list_head_ = nullptr;
    this->list_tail_ = nullptr;
    this->size_ = 0;
  }

  /**
   * A copy constructor that copies the provided list into another instance
   * of that list.
   * @param list The original list to copy from.
   */
  LinkedListArray_(LinkedListArray_ &list) : LinkedListArray_() {
    // Iterate through the provided list in order to copy their data.
    for (size_t index = 0; index < list.size(); index++) {
      DataItem_ item = list.get_item(index);
      this->add_new_item(item);
    }
  }

  /**
   * Deconstructs a linked list of array of data items. The items stored
   * within the array is passed by value, they will automatically deallocate
   * when the array within the linked list node is deallocated.
   */
  ~LinkedListArray_() {
    // Iterate down the linked list and delete them node by node.
    LinkedListArrayNode_ *curr_node = this->list_head_;
    LinkedListArrayNode_ *next_node = nullptr;
    while (curr_node != nullptr) {
      next_node = curr_node->next_;
      delete curr_node;
      // Move to the next node
      curr_node = next_node;
    }
  }

  /**
   * A helper function that adds a new item to the linked list of array of
   * items. This function should not be called by classes that does not
   * inherit from Column.
   * @param item The new Data item that will be added to the linked list.
   */
  void add_new_item(DataItem_ item) {
    // Store the node where the desired array is.
    LinkedListArrayNode_ *curr_node = nullptr;

    // Determine if we have enough linked list nodes for this new item
    if (this->size_ == 0) {
      // This linked list is empty. We need to create a new node for this
      // linked list
      curr_node = new LinkedListArrayNode_();
      this->list_head_ = curr_node;
      this->list_tail_ = curr_node;
    } else if (this->size_ % LinkedListArrayNode_::ARRAY_SIZE == 0) {
      // The current size shows that the latest node is completely full. As a
      // result, we need to add a new node to the linked list
      curr_node = new LinkedListArrayNode_();
      this->list_tail_->next_ = curr_node;
      this->list_tail_ = curr_node;
    } else {
      // The latest node within the linked list has room to add a new item.
      // We'll just take that one.
      curr_node = this->list_tail_;
    }

    // Determine the array index of the new item based off of the size, which
    // is 1-indexed, whereas the array is 0-indexed.
    size_t new_item_index = this->size_ % LinkedListArrayNode_::ARRAY_SIZE;

    // Set the item now
    curr_node->array[new_item_index] = item;

    // Increment the size
    this->size_ += 1;
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
    if ((index > this->size_) || (index < 0)) {
      printf("Index %zu is out of bounds for this column.\n", index);
      exit(1);
    }

    // Now determine which linked list node we need to go to.
    size_t node_index = index / LinkedListArrayNode_::ARRAY_SIZE;

    // Determine which index within the node's array this desired item is.
    size_t array_index = index % LinkedListArrayNode_::ARRAY_SIZE;

    // Now iterate through the linked list in order to get to the right array
    // node
    LinkedListArrayNode_ *curr_node = this->list_head_;
    for (int i = 0; i < node_index; i++) {
      // Move to the next node
      curr_node = curr_node->next_;
    }

    // Now we have the node. Set the value at the specified index
    curr_node->array[array_index] = item;
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
    if ((index > this->size_) || (index < 0)) {
      printf("Index %zu is out of bounds for this column.\n", index);
      exit(1);
    }

    // Now determine which linked list node we need to go to.
    size_t node_index = index / LinkedListArrayNode_::ARRAY_SIZE;

    // Determine which index within the node's array this desired item is.
    size_t array_index = index % LinkedListArrayNode_::ARRAY_SIZE;

    // Now iterate through the linked list in order to get to the right array
    // node
    LinkedListArrayNode_ *curr_node = this->list_head_;
    for (int i = 0; i < node_index; i++) {
      // Move to the next node
      curr_node = curr_node->next_;
    }

    // Now we have the node. Return the stored item
    return curr_node->array[array_index];
  }

  /**
   * Gets the number of items within this data structure.
   * @return The number of items within this data structure.
   */
  size_t size() { return this->size_; }
};

/**
 * A helper class for the helper class LinkedListArray_. It's used to iterate
 * through the data items within the linked list of array in a more efficient
 * manner.
 */
class LinkedListArrayIter_ : public Object {
public:
  LinkedListArray_ *list_;
  size_t index_;
  LinkedListArrayNode_ *curr_node;

  bool have_seen_first;

  /**
   * Constructs an iterator to iterate through the provided list.
   * @param list The list that this iterator should iterate through.
   */
  LinkedListArrayIter_(LinkedListArray_ &list) {
    this->list_ = &list;
    this->curr_node = list.list_head_;
    this->index_ = 0;
    this->have_seen_first = false;
  }

  /**
   * Gets the next data item. Then it will move itself to the next potential
   * item after this one.
   * @return The data item.
   * @throws If there is no item, then it will print out an error message and
   *         terminate the program.
   */
  DataItem_ get_next() {
    if (!this->has_next()) {
      printf("There is no items left in this list.\n");
      exit(1);
    }

    // Get the offset within the array for the node that we are currently on.
    DataItem_ ret_value = this->peek_next();

    // Now move to the next index
    if (!have_seen_first) {
      // Now we have seen the first element. Tip the flag instead of
      // incrementing
      this->have_seen_first = true;
      this->index_ = 0;
    }
    else {
      this->index_ += 1;
      // Move to the next node if the offset is now outside of the bounds of
      // the array for this node.
      size_t offset = this->index_ % LinkedListArrayNode_::ARRAY_SIZE;
      if (offset + 1 >= LinkedListArrayNode_::ARRAY_SIZE) {
        this->curr_node = this->curr_node->next_;
      }
    }

    return ret_value;
  }

  /**
   * Peeks at the next data item. But unlike get_next(), it will not move
   * itself to the next potential item after this one.
   * @return The data item.
   * @throws If there is no item, then it will print out an error message and
   *         terminate the program.
   */
  DataItem_ peek_next() {
    // Get the offset within the array for the node that we are currently on.
    size_t offset = this->index_ % LinkedListArrayNode_::ARRAY_SIZE;

    // Determine if this offset is at the very end of the array for this node.
    // If it is, then look at the first index of the next node, if it exists.
    if (!have_seen_first) {
      // We haven't seen the first element yet. Look at that, if there is an
      // element there.
      if (this->list_->size() > 0) {
        return this->curr_node->array[0];
      }
    }
    if (offset + 1 >= LinkedListArrayNode_::ARRAY_SIZE) {
      LinkedListArrayNode_ *next = this->curr_node->next_;
      if (next == nullptr) {
        printf("There is no items left in this list.\n");
        exit(1);
      } else {
        return next->array[0];
      }
    } else if (this->index_ + 1 >= this->list_->size()) {
      // There are no items valid in the next offset for this array node.
      printf("There is no items left in this list.\n");
      exit(1);
    } else {
      return this->curr_node->array[offset + 1];
    }
  }

  /**
   * Determines if there is a next available item.
   * @return True, if there is, and get_next and peek_next will be able to
   *        properly return with an item. False if there is no item to be
   *        returned as a next item.
   */
  bool has_next() {
    if (this->list_->size() > 0) {
      return ((!have_seen_first) || (this->index_ + 1 < this->list_->size()));
    } else {
      return false;
    }
  }
};

// Lang::CwC
#pragma once

#include <cstdio>

#include "custom_object.h"
#include "thread.h"

/**
 *  The CwC implementation of a Queue supports Objects and (since String is a
 * subclass), Strings. It represents a queue that can be added to and removed
 * from in First-In-First-Out order. That is, the order of objects returned by
 * dequeue() will be exactly the same as the order added by enqueue().
 *  Additionally, this queue implementation does not take ownership of any of
 * its object entries. Objects must be guaranteed by user code to be valid
 * during the lifetime of this queue, and must also be freed by user code.
 */
class Queue : public CustomObject {
public:
  // Default initial capacity for queue
  static const unsigned long DEFAULT_CAPACITY = 16;

  Queue();
  explicit Queue(unsigned long initial_capacity);
  ~Queue() override;

  /**
   * Returns true if the Queue is empty
   * @return true or false
   */
  virtual bool isEmpty() { return (this->size == 0); };

  /**
   * Gets the number of items currently in the queue.
   * @return The number of objects
   */
  virtual unsigned long getSize() { return this->size; }

  /**
   * Adds an item to the top of a Queue.
   * This does not take ownership of the object o, user code must free the
   * object later. o must also be valid for the lifetime of this queue.
   * @param o Object to add (non-null, or this method will abort)
   */
  virtual void enqueue(CustomObject *o);

  /**
   * Removes and returns the object at the bottom of the queue.
   * @returns The object, or null if the queue is empty
   */
  virtual CustomObject *dequeue();

  /**
   * Return the Object on the top (last in) of the Queue
   * @return Object on top of the Queue, or null if the queue is empty
   */
  virtual CustomObject *getTop();

  /**
   * Return Object on bottom (first in) of queue
   * @return Object on bottom of the Queue, or null if the queue is empty
   */
  virtual CustomObject *getBottom();

  /**
   * Checks if an object that is equal (via ->equals()) to the given object
   * currently exists in the queue.
   * @param object What to look for
   * @return true if exists, false if not
   */
  virtual bool contains(CustomObject *object);

  unsigned long hash_me() override;
  bool equals(CustomObject *other) const override;

  /**
   * Resizes the queue to the desired size.
   * @param desired_capacity The desired size.
   * @throws If the desired size is smaller than the number of items in this
   *         queue, then throw an error onto stdout and terminate the program.
   */
  void resize(size_t desired_capacity);

  /**
   * Waits for the queue to have at least one item.
   */
  void wait_for_items();

private:
  unsigned long curr_capacity; // The current maximum of items this queue can
  // store.
  unsigned long size; // The current number of items in this queue

  CustomObject **buffer; // The buffer to store the data.
  size_t curr_head;      // Index of the current head of the queue
  size_t curr_tail;      // Index of the current tail of the queue

  Lock enqueue_signal;
};

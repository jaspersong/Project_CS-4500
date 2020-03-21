// Lang::CwC
#pragma once

#include <stdio.h>

#include "custom_object.h"

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

  unsigned long curr_capacity_; // The current maximum of items this queue can
                                // store.
  unsigned long size_;          // The current number of items in this queue

  CustomObject **buffer_; // The buffer to store the data.
  size_t curr_head_;      // Index of the current head of the queue
  size_t curr_tail_;      // Index of the current tail of the queue

  /**
   * Constructs a new Queue object with a default initial capacity.
   */
  Queue() : Queue(DEFAULT_CAPACITY) {}

  /**
   * Constructs a new Queue object.
   * @param initial_capacity The initial capacity to allocate for storing queue
   * items ie, enqueue can be called this many times initially without Queue
   * having to perform a reallocation
   */
  Queue(unsigned long initial_capacity) : CustomObject() {
    this->curr_capacity_ = initial_capacity;
    this->size_ = 0;

    this->buffer_ = new CustomObject *[this->curr_capacity_];
    this->curr_head_ = 0;
    this->curr_tail_ = 0;
  }

  /**
   * Destructor for Queue. Does not free any contained objects (that is the
   * responsibility of user code).
   */
  virtual ~Queue() {
    this->curr_capacity_ = 0;
    this->size_ = 0;

    delete[] this->buffer_;
    this->buffer_ = nullptr;
    this->curr_head_ = 0;
    this->curr_tail_ = 0;
  }

  /**
   * Returns true if the Queue is empty
   * @return true or false
   */
  virtual bool isEmpty() { return (this->size_ == 0); };

  /**
   * Gets the number of items currently in the queue.
   * @return The number of objects
   */
  virtual unsigned long getSize() { return this->size_; }

  /**
   * Adds an item to the top of a Queue.
   * This does not take ownership of the object o, user code must free the
   * object later. o must also be valid for the lifetime of this queue.
   * @param o Object to add (non-null, or this method will abort)
   */
  virtual void enqueue(CustomObject *o) {
    if (this->size_ + 1 > this->curr_capacity_) {
      // Not enough room to add the object. Resize quadratically to
      // compensate
      this->resize(this->curr_capacity_ * this->curr_capacity_);
    }

    // Place the object at where the current tail is
    this->buffer_[this->curr_tail_] = o;
    this->size_ += 1;

    // Now increment the tail, taking into account the rollover
    this->curr_tail_ += 1;
    if (this->curr_tail_ >= this->curr_capacity_) {
      this->curr_tail_ = this->curr_tail_ - this->curr_capacity_;
    }
  }

  /**
   * Removes and returns the object at the bottom of the queue.
   * @returns The object, or null if the queue is empty
   */
  virtual CustomObject *dequeue() {
    if (this->size_ == 0) {
      return nullptr;
    } else {
      // Get the object from the queue
      CustomObject *ret_value = this->buffer_[this->curr_head_];
      this->size_ -= 1;

      // Now increment the head, taking into account the rollover
      this->curr_head_ += 1;
      if (this->curr_head_ >= this->curr_capacity_) {
        this->curr_head_ = this->curr_head_ - this->curr_capacity_;
      }

      return ret_value;
    }
  }

  /**
   * Return the Object on the top (last in) of the Queue
   * @return Object on top of the Queue, or null if the queue is empty
   */
  virtual CustomObject *getTop() {
    if (this->size_ == 0) {
      return nullptr;
    } else {
      // The last item is the previous index to the current tail.
      if (this->curr_tail_ == 0) {
        // This is the beginning of the buffer. The last item will be at the
        // other end of the buffer.
        return this->buffer_[this->curr_capacity_ - 1];
      } else {
        return this->buffer_[this->curr_tail_ - 1];
      }
    }
  }

  /**
   * Return Object on bottom (first in) of queue
   * @return Object on bottom of the Queue, or null if the queue is empty
   */
  virtual CustomObject *getBottom() {
    if (this->size_ == 0) {
      return nullptr;
    } else {
      return this->buffer_[this->curr_head_];
    }
  }

  /**
   * Checks if an object that is equal (via ->equals()) to the given object
   * currently exists in the queue.
   * @param object What to look for
   * @return true if exists, false if not
   */
  virtual bool contains(CustomObject *object) {
    // Iterate through the queue until we find the object
    size_t curr_index = this->curr_head_;
    while (curr_index != this->curr_tail_) {
      if (this->buffer_[curr_index]->equals(object)) {
        return true;
      }

      // Move to the next index, taking into account rollover
      curr_index += 1;
      if (curr_index >= this->curr_capacity_) {
        curr_index = curr_index - this->curr_capacity_;
      }
    }

    // Couldn't find it.
    return false;
  }

  // Force implementation of hash and equals for the queue
  virtual unsigned long hash() {
    unsigned long hash = this->size_;

    // Iterate through the queue, building the hash based off of the hashes
    // of the stored objects.
    size_t curr_index = this->curr_head_;
    while (curr_index != this->curr_tail_) {
      hash += this->buffer_[curr_index]->hash();

      // Move to the next index, taking into account rollover
      curr_index += 1;
      if (curr_index >= this->curr_capacity_) {
        curr_index = curr_index - this->curr_capacity_;
      }
    }

    return hash;
  }

  virtual bool equals(CustomObject *other) {
    if (other == nullptr) {
      return false;
    }

    // Try to cast the other item into queue
    Queue *q = dynamic_cast<Queue *>(other);

    if (q == nullptr) {
      return false;
    } else if (this->size_ == q->size_) {
      // The two queues are of the same size. Iterate through the objects
      // to see if they are made up of the same objects.
      size_t this_curr = this->curr_head_;
      size_t that_curr = q->curr_head_;
      while ((this_curr != this->curr_tail_) && (that_curr != q->curr_tail_)) {
        // Ensure that this item and that item are equal.
        if (!this->buffer_[this_curr]->equals(q->buffer_[that_curr])) {
          // Found a mismatch
          return false;
        }

        // Move to the next index, taking into account rollover
        this_curr += 1;
        if (this_curr >= this->curr_capacity_) {
          this_curr = this_curr - this->curr_capacity_;
        }
        that_curr += 1;
        if (that_curr >= q->curr_capacity_) {
          that_curr = that_curr - q->curr_capacity_;
        }
      }

      // They all match.
      return true;
    } else {
      return false;
    }
  }

  /**
   * Resizes the queue to the desired size.
   * @param desired_capacity The desired size.
   * @throws If the desired size is smaller than the number of items in this
   *         queue, then throw an error onto stdout and terminate the program.
   */
  void resize(size_t desired_capacity) {
    if (desired_capacity < this->size_) {
      printf("Cannot shrink a queue.\n");
    }

    // Create the new buffer
    CustomObject **new_buffer = new CustomObject *[desired_capacity];

    // Copy the current values in the old buffer into the new buffer
    size_t old_curr = this->curr_head_;
    size_t new_curr = 0;
    while (old_curr != this->curr_tail_) {
      // Copy the value
      new_buffer[new_curr] = this->buffer_[old_curr];

      // Move to the next index, taking into account rollover
      old_curr += 1;
      if (old_curr >= this->curr_capacity_) {
        old_curr = old_curr - this->curr_capacity_;
      }
      // No need to rollover because it will never reach the end of the new
      // buffer
      new_curr += 1;
    }

    // Now replace the old buffer and update the buffer pointers
    delete[] this->buffer_;
    this->buffer_ = new_buffer;
    this->curr_head_ = 0;
    this->curr_tail_ = new_curr;
    this->curr_capacity_ = desired_capacity;
  }
};

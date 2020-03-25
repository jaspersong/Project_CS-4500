/**
 * Name: Snowy Chen, Joe Song
 * Date: 22 March 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::Cpp

#include "queue.h"

Queue::Queue() : Queue(DEFAULT_CAPACITY) {}

Queue::Queue(unsigned long initial_capacity) : CustomObject() {
  this->curr_capacity = initial_capacity;
  this->size = 0;

  this->buffer = new CustomObject *[this->curr_capacity];
  this->curr_head = 0;
  this->curr_tail = 0;
}

Queue::~Queue() {
  this->curr_capacity = 0;
  this->size = 0;

  delete[] this->buffer;
  this->buffer = nullptr;
  this->curr_head = 0;
  this->curr_tail = 0;
}

void Queue::enqueue(CustomObject *o) {
  if (this->size + 1 > this->curr_capacity) {
    // Not enough room to add the object. Resize quadratically to
    // compensate
    this->resize(this->curr_capacity * this->curr_capacity);
  }

  // Place the object at where the current tail is
  this->buffer[this->curr_tail] = o;
  this->size += 1;

  // Now increment the tail, taking into account the rollover
  this->curr_tail += 1;
  if (this->curr_tail >= this->curr_capacity) {
    this->curr_tail = this->curr_tail - this->curr_capacity;
  }

  // Notify that there is now an item on the queue
  this->enqueue_signal.notify_all();
}

CustomObject *Queue::dequeue() {
  if (this->size == 0) {
    return nullptr;
  } else {
    // Get the object from the queue
    CustomObject *ret_value = this->buffer[this->curr_head];
    this->size -= 1;

    // Now increment the head, taking into account the rollover
    this->curr_head += 1;
    if (this->curr_head >= this->curr_capacity) {
      this->curr_head = this->curr_head - this->curr_capacity;
    }

    return ret_value;
  }
}

CustomObject *Queue::getTop() {
  if (this->size == 0) {
    return nullptr;
  } else {
    // The last item is the previous index to the current tail.
    if (this->curr_tail == 0) {
      // This is the beginning of the buffer. The last item will be at the
      // other end of the buffer.
      return this->buffer[this->curr_capacity - 1];
    } else {
      return this->buffer[this->curr_tail - 1];
    }
  }
}

CustomObject *Queue::getBottom() {
  if (this->size == 0) {
    return nullptr;
  } else {
    return this->buffer[this->curr_head];
  }
}

bool Queue::contains(CustomObject *object) {
  // Iterate through the queue until we find the object
  size_t curr_index = this->curr_head;
  while (curr_index != this->curr_tail) {
    if (this->buffer[curr_index]->equals(object)) {
      return true;
    }

    // Move to the next index, taking into account rollover
    curr_index += 1;
    if (curr_index >= this->curr_capacity) {
      curr_index = curr_index - this->curr_capacity;
    }
  }

  // Couldn't find it.
  return false;
}

unsigned long Queue::hash_me() {
  unsigned long hash = this->size;

  // Iterate through the queue, building the hash based off of the hashes
  // of the stored objects.
  size_t curr_index = this->curr_head;
  while (curr_index != this->curr_tail) {
    hash += this->buffer[curr_index]->hash();

    // Move to the next index, taking into account rollover
    curr_index += 1;
    if (curr_index >= this->curr_capacity) {
      curr_index = curr_index - this->curr_capacity;
    }
  }

  return hash;
}

bool Queue::equals(CustomObject *other) {
  if (other == nullptr) {
    return false;
  }

  // Try to cast the other item into queue
  auto *q = dynamic_cast<Queue *>(other);

  if (q == nullptr) {
    return false;
  } else if (this->size == q->size) {
    // The two queues are of the same size. Iterate through the objects
    // to see if they are made up of the same objects.
    size_t this_curr = this->curr_head;
    size_t that_curr = q->curr_head;
    while ((this_curr != this->curr_tail) && (that_curr != q->curr_tail)) {
      // Ensure that this item and that item are equal.
      if (!this->buffer[this_curr]->equals(q->buffer[that_curr])) {
        // Found a mismatch
        return false;
      }

      // Move to the next index, taking into account rollover
      this_curr += 1;
      if (this_curr >= this->curr_capacity) {
        this_curr = this_curr - this->curr_capacity;
      }
      that_curr += 1;
      if (that_curr >= q->curr_capacity) {
        that_curr = that_curr - q->curr_capacity;
      }
    }

    // They all match.
    return true;
  } else {
    return false;
  }
}

void Queue::resize(size_t desired_capacity) {
  if (desired_capacity < this->size) {
    printf("Cannot shrink a queue.\n");
  }

  // Create the new buffer
  auto **new_buffer = new CustomObject *[desired_capacity];

  // Copy the current values in the old buffer into the new buffer
  size_t old_curr = this->curr_head;
  size_t new_curr = 0;
  while (old_curr != this->curr_tail) {
    // Copy the value
    new_buffer[new_curr] = this->buffer[old_curr];

    // Move to the next index, taking into account rollover
    old_curr += 1;
    if (old_curr >= this->curr_capacity) {
      old_curr = old_curr - this->curr_capacity;
    }
    // No need to rollover because it will never reach the end of the new
    // buffer
    new_curr += 1;
  }

  // Now replace the old buffer and update the buffer pointers
  delete[] this->buffer;
  this->buffer = new_buffer;
  this->curr_head = 0;
  this->curr_tail = new_curr;
  this->curr_capacity = desired_capacity;
}

void Queue::wait_for_items() {
  if (this->getSize() < 1) {
    // Wait only if there is nothing in the queue
    this->enqueue_signal.wait();
  }
}

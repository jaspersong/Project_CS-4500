/**
 * Name: Snowy Chen, Joe Song
 * Date: 21 February 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::CwC

#pragma once

#include <arpa/inet.h>
#include <netinet/in.h>

#include "../../string.h"
#include "serial.h"

/**
 * A non-growing array of strings. All of the strings stored in the array
 * will remain external.
 */
class StringArray : public CustomObject {
public:
  String **vals_;
  size_t capacity_;

  /**
   * Constructs a string array. All of the values will be initialized as a
   * nullptr.
   * @param capacity The maximum number of strings this array will hold.
   */
  explicit StringArray(size_t capacity) {
    this->capacity_ = capacity;
    this->vals_ = new String *[capacity];
  }

  /**
   * Deconstructs the string array.
   */
  ~StringArray() override { delete[] this->vals_; }

  /**
   * Gets the string at the provided index.
   * @param index The index of the desired string.
   * @return The string stored in the array.
   * @throws If the index is invalid, it will terminate the program.
   */
  String *get(size_t index) {
    assert(index >= 0);
    assert(index < this->capacity_);

    return this->vals_[index];
  }

  /**
   * Sets the value at the provided index.
   * @param index The index to store the provided value.
   * @param value The value to store. The string will be external. It can
   *        also be nullptr.
   * @throws If the index is invalid, it will terminate the program.
   */
  void set(size_t index, String *value) {
    assert(index >= 0);
    assert(index < this->capacity_);

    this->vals_[index] = value;
  }

  /**
   * Serializes the array into the provided buffer.
   * @param buffer The pointer to the buffer.
   * @param offset A number of bytes to offset from the provided buffer to
   *        start writing the values in.
   * @param buffer_size The size of the provided buffer.
   * @return The offset + the number of bytes used to serialize the array.
   * @throws If there is not enough space in the buffer, or the buffer is
   *         invalid, the program will terminate. If the buffer size provided
   *         is not reflective of the actual buffer, the behavior is undefined
   */
  size_t serialize(unsigned char *buffer, size_t offset, size_t buffer_size) {
    assert(buffer != nullptr);
    assert(offset <= buffer_size);

    // Set the values
    size_t ser_offset = Message::set_size_t_serialization(
        this->capacity_, buffer, offset, buffer_size);
    for (size_t i = 0; i < this->capacity_; i++) {
      if (this->vals_[i] == nullptr) {
        // Set a "-1" string in order to denote that this is a nullptr.
        ser_offset = Message::set_size_t_serialization(-1, buffer, ser_offset,
                                                       buffer_size);
      } else {
        ser_offset = Message::set_string_serialization(this->vals_[i], buffer,
                                                       ser_offset, buffer_size);
      }
    }

    return ser_offset;
  }

  /**
   * Takes in the provided buffer and interprets it as an array of strings.
   * @param buffer The buffer to deserialize_as_message from.
   * @param buffer_size The size of the buffer.
   * @return A newly allocated StringArray that was deserialized from the
   *        buffer. If it failed to interpret the provided buffer as a String
   *        array, it will return with nullptr.
   * @throws If the buffer/buffer size is invalid, it will terminate the
   *        program. If the buffer size is not reflective of the buffer, the
   *        behavior is undefined.
   */
  static StringArray *deserialize(unsigned char *buffer, size_t buffer_size) {
    assert(buffer != nullptr);

    size_t offset = 0;
    size_t capacity =
        Message::get_size_t_deserialization(buffer, offset, buffer_size);

    StringArray *ret_value = new StringArray(capacity);
    for (size_t i = 0; i < capacity; i++) {
      // Peek at the size to see if this is a nullptr string value.
      size_t peek_offset = offset;
      size_t peek_size =
          Message::get_size_t_deserialization(buffer, peek_offset, buffer_size);
      if (peek_size == -1) {
        // This is a nullptr. Move the offset to the peeked offset as well.
        ret_value->vals_[i] = nullptr;
        offset = peek_offset;
      } else {
        // Get the string value
        String *value =
            Message::get_string_deserialization(buffer, offset, buffer_size);
        ret_value->vals_[i] = value;
      }
    }

    return ret_value;
  }

  bool equals(CustomObject *other) override {
    if (other == this)
      return true;
    StringArray *x = dynamic_cast<StringArray *>(other);
    if (x == nullptr)
      return false;

    if (this->capacity_ != x->capacity_)
      return false;

    for (size_t i = 0; i < this->capacity_; i++) {
      if (!this->vals_[i]->equals(x->vals_[i]))
        return false;
    }
    return true;
  }

  size_t hash_me() override {
    size_t ret_value = this->capacity_;
    for (size_t i = 0; i < this->capacity_; i++) {
      ret_value += this->vals_[i]->hash_me();
    }

    return ret_value;
  }
};

class DoubleArray : public CustomObject {
public:
  size_t capacity_;
  double *vals_;

  /**
   * Constructs a double array. All of the values will be initialized as 0.
   * @param capacity The maximum number of doubles this array will hold.
   */
  explicit DoubleArray(size_t capacity) {
    this->capacity_ = capacity;
    this->vals_ = new double[capacity];
    for (size_t i = 0; i < capacity; i++) {
      this->vals_[i] = 0.0;
    }
  }

  /**
   * Deconstructs the double array.
   */
  ~DoubleArray() override { delete[] this->vals_; }

  /**
   * Gets the value at the provided index.
   * @param index The index of the desired string.
   * @return The value stored in the array.
   * @throws If the index is invalid, it will terminate the program.
   */
  double get(size_t index) {
    assert(index >= 0);
    assert(index < this->capacity_);

    return this->vals_[index];
  }

  /**
   * Sets the value at the provided index.
   * @param index The index to store the provided value.
   * @param value The value to store.
   * @throws If the index is invalid, it will terminate the program.
   */
  void set(size_t index, double value) {
    assert(index >= 0);
    assert(index < this->capacity_);

    this->vals_[index] = value;
  }

  /**
   * Serializes the array into the provided buffer.
   * @param buffer The pointer to the buffer.
   * @param offset A number of bytes to offset from the provided buffer to
   *        start writing the values in.
   * @param buffer_size The size of the provided buffer.
   * @return The offset + the number of bytes used to serialize the array.
   * @throws If there is not enough space in the buffer, or the buffer is
   *         invalid, the program will terminate. If the buffer size provided
   *         is not reflective of the actual buffer, the behavior is undefined
   */
  size_t serialize(unsigned char *buffer, size_t offset, size_t buffer_size) {
    assert(buffer != nullptr);
    assert(offset <= buffer_size);

    // Set the values
    size_t ser_offset = Message::set_size_t_serialization(
        this->capacity_, buffer, offset, buffer_size);
    for (size_t i = 0; i < this->capacity_; i++) {
      ser_offset = Message::set_double_serialization(this->vals_[i], buffer,
                                                     ser_offset, buffer_size);
    }

    return ser_offset;
  }

  /**
   * Takes in the provided buffer and interprets it as an array of strings.
   * @param buffer The buffer to deserialize_as_message from.
   * @param buffer_size The size of the buffer.
   * @return A newly allocated DoubleArray that was deserialized from the
   *        buffer. If it failed to interpret the provided buffer as a String
   *        array, it will return with nullptr.
   * @throws If the buffer/buffer size is invalid, it will terminate the
   *        program. If the buffer size is not reflective of the buffer, the
   *        behavior is undefined.
   */
  static DoubleArray *deserialize(unsigned char *buffer, size_t buffer_size) {
    assert(buffer != nullptr);

    size_t offset = 0;
    size_t capacity =
        Message::get_size_t_deserialization(buffer, offset, buffer_size);

    DoubleArray *ret_value = new DoubleArray(capacity);
    for (size_t i = 0; i < capacity; i++) {
      ret_value->vals_[i] =
          Message::get_double_deserialization(buffer, offset, buffer_size);
    }

    return ret_value;
  }

  bool equals(CustomObject *other) override {
    if (other == this)
      return true;
    DoubleArray *x = dynamic_cast<DoubleArray *>(other);
    if (x == nullptr)
      return false;
    if (this->capacity_ != x->capacity_)
      return false;

    for (size_t i = 0; i < this->capacity_; i++) {
      if (this->vals_[i] != x->vals_[i]) {
        return false;
      }
    }
    return true;
  }

  size_t hash_me() override {
    size_t ret_value = this->capacity_;
    for (size_t i = 0; i < this->capacity_; i++) {
      ret_value += static_cast<size_t>(this->vals_[i]);
    }

    return ret_value;
  }
};

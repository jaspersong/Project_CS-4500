/**
 * Name: Snowy Chen, Joe Song
 * Date: 22 March 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::Cpp

#include <cassert>
#include <cstring>

#include "serializer.h"

Serializer::Serializer(size_t starting_size) {
  this->buffer = new unsigned char[starting_size];
  this->buffer_size = starting_size;
  this->offset = 0;
  this->owns_buffer = true;
}

Serializer::~Serializer() {
  if (this->owns_buffer) {
    delete[] this->buffer;
  }
  this->buffer = nullptr;
  this->buffer_size = 0;
  this->offset = 0;
}

bool Serializer::grow_buffer() {
  if (this->owns_buffer) {
    // Reallocate the buffer
    size_t new_size = this->buffer_size * this->buffer_size;
    auto *new_buffer = new unsigned char[new_size];

    // Copy the previous data into
    memcpy(static_cast<void *>(new_buffer), static_cast<void *>(this->buffer),
           this->offset);

    // Now replace the old buffer with the new one
    delete[] this->buffer;
    this->buffer = new_buffer;
    this->buffer_size = new_size;

    return true;
  } else {
    return false;
  }
}

bool Serializer::set_generic(unsigned char *value, size_t num_bytes) {
  assert(value != nullptr);

  // Make sure there's enough space in the buffer for this value
  bool ret_value = (this->buffer_size - this->offset) >= num_bytes;
  // Make sure that we own the buffer as well
  ret_value = this->owns_buffer && ret_value;

  // Grow the buffer if necessary
  if (!ret_value && this->owns_buffer) {
    ret_value = this->grow_buffer();
  }

  // Set the value if possible
  if (ret_value) {
    auto *value_buffer = static_cast<unsigned char *>(value);
    memcpy(this->buffer + this->offset, value_buffer, num_bytes);

    // Move the offset
    this->offset += num_bytes;

    ret_value = true;
  }

  return ret_value;
}

unsigned char *Serializer::get_serialized_buffer() {
  this->owns_buffer = false;
  return this->buffer;
}

Deserializer::Deserializer(unsigned char *buffer, size_t num_bytes,
                           bool steal) {
  assert(buffer != nullptr);

  if (steal) {
    this->buffer = buffer;
    this->num_bytes = num_bytes;
    this->offset = 0;
  } else {
    this->num_bytes = num_bytes;
    this->offset = 0;
    this->buffer = new unsigned char[this->num_bytes];
    memcpy(this->buffer, reinterpret_cast<const void *>(buffer),
           this->num_bytes);
  }

  this->own_buffer = true;
}

Deserializer::Deserializer(unsigned char *buffer, size_t num_bytes) {
  assert(buffer != nullptr);

  this->buffer = buffer;
  this->num_bytes = num_bytes;
  this->offset = 0;

  this->own_buffer = false;
}

Deserializer::~Deserializer() {
  if (this->own_buffer) {
    delete[] this->buffer;
  }
  this->buffer = nullptr;
  this->num_bytes = 0;
  this->offset = 0;
}

size_t Deserializer::get_num_bytes_left() {
  if (this->offset <= this->num_bytes) {
    return this->num_bytes - this->offset;
  } else {
    return 0;
  }
}

unsigned char Deserializer::get_byte() {
  assert(this->get_num_bytes_left() >= sizeof(unsigned char));

  unsigned char ret_value = this->buffer[this->offset];

  // Move to the next pointer
  this->offset += sizeof(unsigned char);

  return ret_value;
}

bool Deserializer::get_bool() {
  assert(this->get_num_bytes_left() >= sizeof(bool));

  // Cast the buffer into an array of bool in order to grab the binary of
  // the bool for the next few bytes
  bool *interpreted_buffer =
      reinterpret_cast<bool *>(this->buffer + this->offset);
  bool ret_value = interpreted_buffer[0];

  // Update the offset to a new offset based off the string
  this->offset += sizeof(bool);

  return ret_value;
}

size_t Deserializer::get_size_t() {
  assert(this->get_num_bytes_left() >= sizeof(size_t));

  // Get the size of the string
  auto *size_t_buffer = reinterpret_cast<size_t *>(this->buffer + this->offset);
  size_t ret_value = size_t_buffer[0];

  // Update the offset to a new offset based off the string
  this->offset += sizeof(size_t);

  return ret_value;
}

int Deserializer::get_int() {
  assert(this->get_num_bytes_left() >= sizeof(int));

  // Cast the buffer into an array of bool in order to grab the binary of
  // the bool for the next few bytes
  int *interpreted_buffer =
      reinterpret_cast<int *>(this->buffer + this->offset);
  int ret_value = interpreted_buffer[0];

  // Update the offset to a new offset based off the string
  this->offset += sizeof(int);

  return ret_value;
}

double Deserializer::get_double() {
  assert(this->get_num_bytes_left() >= sizeof(double));

  // Cast the buffer into an array of bool in order to grab the binary of
  // the bool for the next few bytes
  auto *interpreted_buffer =
      reinterpret_cast<double *>(this->buffer + this->offset);
  double ret_value = interpreted_buffer[0];

  // Update the offset to a new offset based off the string
  this->offset += sizeof(double);

  return ret_value;
}

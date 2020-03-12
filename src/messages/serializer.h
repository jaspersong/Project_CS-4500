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

/**
 * A helper class that assists with serializing data. It is meant to assist
 * with serializing Object classes into an array of bytes. This can be
 * returned by calling the serialize() function within Object, or the Object can
 * add its data after previously serialized data by passing a Serializer into
 * the serialize() function.
 *
 * All serialized data is in a binary format. Thus, different data types cannot
 * be distinguished dynamically except through expected identifiers that are
 * serialized into the data itself.
 */
class Serializer {
public:
  static const size_t DEFAULT_STARTING_BUFFER_SIZE = 32;

  bool owns_buffer_;
  unsigned char *buffer_;
  size_t buffer_size_;
  size_t offset_;

  /**
   * Constructs a serializer that starts at the specified size buffer.
   * @param starting_size Starting size of teh buffer.
   */
  explicit Serializer(size_t starting_size) {
    this->buffer_ = new unsigned char[starting_size];
    this->buffer_size_ = starting_size;
    this->offset_ = 0;
    this->owns_buffer_ = true;
  }

  /**
   * Constructs a serializer.
   */
  Serializer() : Serializer(Serializer::DEFAULT_STARTING_BUFFER_SIZE) {}

  /**
   * Deconstructs a serializer.
   */
  ~Serializer() {
    if (this->owns_buffer_) {
      delete[] this->buffer_;
    }
    this->buffer_ = nullptr;
    this->buffer_size_ = 0;
    this->offset_ = 0;
  }

  /**
   * A private helper function intended to grow the serialized buffer
   * quadratically. This function will only work if the serializer owns the
   * buffer.
   * @return True if it was able to grow the buffer. False if otherwise.
   */
  bool grow_buffer_() {
    if (this->owns_buffer_) {
      // Reallocate the buffer
      size_t new_size = this->buffer_size_ * this->buffer_size_;
      unsigned char *new_buffer = new unsigned char[new_size];

      // Copy the previous data into
      memcpy(new_buffer, this->buffer_, this->offset_);

      // Now replace the old buffer with the new one
      delete[] this->buffer_;
      this->buffer_ = new_buffer;
      this->buffer_size_ = new_size;

      return true;
    } else {
      return false;
    }
  }

  /**
   * Calculates the number of bytes is needed to serialize the provided value.
   * @param value The value to serialize.
   * @return The number of bytes needed to serialize the provided value.
   */
  static size_t get_required_bytes(bool value) { return sizeof(bool); }

  /**
   * Calculates the number of bytes is needed to serialize the provided value.
   * @param value The value to serialize.
   * @return The number of bytes needed to serialize the provided value.
   */
  static size_t get_required_bytes(size_t value) { return sizeof(size_t); }

  /**
   * Calculates the number of bytes is needed to serialize the provided value.
   * @param value The value to serialize.
   * @return The number of bytes needed to serialize the provided value.
   */
  static size_t get_required_bytes(int value) { return sizeof(int); }

  /**
   * Calculates the number of bytes is needed to serialize the provided value.
   * @param value The value to serialize.
   * @return The number of bytes needed to serialize the provided value.
   */
  static size_t get_required_bytes(double value) { return sizeof(double); }

  /**
   * Appends the serialization of the provided bool value into the serialized
   * data buffer.
   * @param value The value to serialize into the serialized data buffer.
   * @return True if it successfully serialized the data. False if otherwise.
   */
  bool set_bool(bool value) {
    return this->set_generic(reinterpret_cast<void *>(&value), sizeof(value));
  }

  /**
   * Appends the serialization of the provided size_t value into the serialized
   * data buffer.
   * @param value The value to serialize into the serialized data buffer.
   * @return True if it successfully serialized the data. False if otherwise.
   */
  bool set_size_t(size_t value) {
    return this->set_generic(reinterpret_cast<void *>(&value), sizeof(value));
  }

  /**
   * Appends the serialization of the provided int value into the serialized
   * data buffer.
   * @param value The value to serialize into the serialized data buffer.
   * @return True if it successfully serialized the data. False if otherwise.
   */
  bool set_int(int value) {
    return this->set_generic(reinterpret_cast<void *>(&value), sizeof(value));
  }

  /**
   * Appends the serialization of the provided double value into the serialized
   * data buffer.
   * @param value The value to serialize into the serialized data buffer.
   * @return True if it successfully serialized the data. False if otherwise.
   */
  bool set_double(double value) {
    return this->set_generic(reinterpret_cast<void *>(&value), sizeof(value));
  }

  /**
   * Appends a generic value into the serialized data buffer. If the number
   * of bytes is not accurate to the size of the value, the behavior is
   * undefined.
   * @param value A pointer to the value. This cannot be null or else it will
   *       terminate the program.
   * @param num_bytes The number of bytes the value is.
   * @return True if it successfully serialized the data. False if otherwise.
   */
  bool set_generic(void *value, size_t num_bytes) {
    assert(value != nullptr);

    // Make sure there's enough space in the buffer for this value
    bool ret_value = (this->buffer_size_ - this->offset_) >= num_bytes;
    // Make sure that we own the buffer as well
    ret_value = this->owns_buffer_ && ret_value;

    // Grow the buffer if necessary
    if (!ret_value && this->owns_buffer_) {
      ret_value = this->grow_buffer_();
    }

    // Set the value if possible
    if (ret_value) {
      unsigned char *value_buffer = static_cast<unsigned char *>(value);
      memcpy(this->buffer_ + this->offset_, value_buffer, num_bytes);

      // Move the offset
      this->offset_ += num_bytes;

      ret_value = true;
    }

    return ret_value;
  }

  /**
   * Returns the buffer containing the serialized data. Once this function
   * gets called, the caller is now the owner of the buffer.
   * @return The buffer containing the serialized data.
   */
  unsigned char *get_serialized_buffer() {
    this->owns_buffer_ = false;
    return this->buffer_;
  }

  /**
   * Gets the number of bytes the serialized data takes up.
   * @return The size of the serialized data.
   */
  size_t get_size_serialized_data() { return this->offset_; }
};

/**
 * Helper class that assists with deserializing data. It does not know what
 * data type or object is serialized within the buffer, as all data is binary.
 * Thus, the caller of the deserializer must know what format or type the
 * serialized data contains.
 */
class Deserializer {
public:
  unsigned char *buffer; // Does not own
  size_t num_bytes;
  size_t offset_;

  /**
   * Constructs a deserializer.
   * @param buffer The buffer to deserialize_as_message from. It will not be
   * modified by the deserializer, and the deserializer will not own the buffer.
   * @param num_bytes The number of bytes the buffer is.
   * @throws If the buffer provided is null, it will terminate the program.
   */
  Deserializer(unsigned char *buffer, size_t num_bytes) {
    assert(buffer != nullptr);

    this->buffer = buffer;
    this->num_bytes = num_bytes;
    this->offset_ = 0;
  }

  /**
   * Deconstructs a deserializer.
   */
  ~Deserializer() {
    this->buffer = nullptr;
    this->num_bytes = 0;
    this->offset_ = 0;
  }

  /**
   * Gets the number of bytes that are available to be read in the serialized
   * data.
   * @return The number of bytes that are available to be read.
   */
  size_t get_num_bytes_left() {
    if (this->offset_ <= this->num_bytes) {
      return this->num_bytes - this->offset_;
    } else {
      return 0;
    }
  }

  /**
   * Determines if there is enough bytes left to be read from the buffer in
   * order to retrieve a data value of a byte.
   * @return True if there is enough data left to read. False if otherwise.
   */
  bool has_byte() {
    return this->get_num_bytes_left() >= sizeof(unsigned char);
  }

  /**
   * Determines if there is enough bytes left to be read from the buffer in
   * order to retrieve a data value of a bool.
   * @return True if there is enough data left to read. False if otherwise.
   */
  bool has_bool() { return this->get_num_bytes_left() >= sizeof(bool); }

  /**
   * Determines if there is enough bytes left to be read from the buffer in
   * order to retrieve a data value of a size_t.
   * @return True if there is enough data left to read. False if otherwise.
   */
  bool has_size_t() { return this->get_num_bytes_left() >= sizeof(size_t); }

  /**
   * Determines if there is enough bytes left to be read from the buffer in
   * order to retrieve a data value of a int.
   * @return True if there is enough data left to read. False if otherwise.
   */
  bool has_int() { return this->get_num_bytes_left() >= sizeof(int); }

  /**
   * Determines if there is enough bytes left to be read from the buffer in
   * order to retrieve a data value of a double.
   * @return True if there is enough data left to read. False if otherwise.
   */
  bool has_double() { return this->get_num_bytes_left() >= sizeof(double); }

  /**
   * Gets the next byte of buffer. It will move the pointer within the
   * buffer, so calling this function again will read the next byte of the
   * buffer.
   * @return The next byte of the buffer.
   * @throws Throws an error and terminates the program if there is not
   *        enough bytes left to be able to read from the buffer.
   */
  unsigned char get_byte() {
    assert(this->get_num_bytes_left() >= sizeof(unsigned char));

    unsigned char ret_value = this->buffer[this->offset_];

    // Move to the next pointer
    this->offset_ += sizeof(unsigned char);

    return ret_value;
  }

  /**
   * Gets the next few bytes as a bool data type. It will move the pointer
   * within the buffer, so calling this function again will read the next
   * bool of the buffer.
   * @return The next bytes as a bool value.
   * @throws Throws an error and terminates the program if there is not
   *         enough bytes left to be able to read a bool value from the
   *         buffer.
   */
  bool get_bool() {
    assert(this->get_num_bytes_left() >= sizeof(bool));

    // Cast the buffer into an array of bool in order to grab the binary of
    // the bool for the next few bytes
    bool *interpreted_buffer =
        reinterpret_cast<bool *>(this->buffer + this->offset_);
    bool ret_value = interpreted_buffer[0];

    // Update the offset to a new offset based off the string
    this->offset_ += sizeof(bool);

    return ret_value;
  }

  /**
   * Gets the next few bytes as a size_t data type. It will move the pointer
   * within the buffer, so calling this function again will read the next
   * size_t of the buffer.
   * @return The next bytes as a size_t value.
   * @throws Throws an error and terminates the program if there is not
   *         enough bytes left to be able to read a size_t value from the
   *         buffer.
   */
  size_t get_size_t() {
    assert(this->get_num_bytes_left() >= sizeof(size_t));

    // Get the size of the string
    size_t *size_t_buffer =
        reinterpret_cast<size_t *>(this->buffer + this->offset_);
    size_t ret_value = size_t_buffer[0];

    // Update the offset to a new offset based off the string
    this->offset_ += sizeof(size_t);

    return ret_value;
  }

  /**
   * Gets the next few bytes as a int data type. It will move the pointer
   * within the buffer, so calling this function again will read the next
   * int of the buffer.
   * @return The next bytes as a int value.
   * @throws Throws an error and terminates the program if there is not
   *         enough bytes left to be able to read a int value from the
   *         buffer.
   */
  int get_int() {
    assert(this->get_num_bytes_left() >= sizeof(int));

    // Cast the buffer into an array of bool in order to grab the binary of
    // the bool for the next few bytes
    int *interpreted_buffer =
        reinterpret_cast<int *>(this->buffer + this->offset_);
    int ret_value = interpreted_buffer[0];

    // Update the offset to a new offset based off the string
    this->offset_ += sizeof(int);

    return ret_value;
  }

  /**
   * Gets the next few bytes as a double data type. It will move the pointer
   * within the buffer, so calling this function again will read the next
   * double of the buffer.
   * @return The next bytes as a double value.
   * @throws Throws an error and terminates the program if there is not
   *         enough bytes left to be able to read a double value from the
   *         buffer.
   */
  double get_double() {
    assert(this->get_num_bytes_left() >= sizeof(double));

    // Cast the buffer into an array of bool in order to grab the binary of
    // the bool for the next few bytes
    double *interpreted_buffer =
        reinterpret_cast<double *>(this->buffer + this->offset_);
    double ret_value = interpreted_buffer[0];

    // Update the offset to a new offset based off the string
    this->offset_ += sizeof(double);

    return ret_value;
  }
};

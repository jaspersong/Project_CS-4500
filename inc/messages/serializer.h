/**
 * Name: Snowy Chen, Joe Song
 * Date: 21 February 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::CwC

#pragma once

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

  explicit Serializer(size_t starting_size);
  Serializer() : Serializer(Serializer::DEFAULT_STARTING_BUFFER_SIZE) {}
  ~Serializer();

  /**
   * Calculates the number of bytes is needed to serialize the provided value.
   * @param value The value to serialize.
   * @return The number of bytes needed to serialize the provided value.
   */
  static size_t get_required_bytes(bool value) { return sizeof(bool); }
  static size_t get_required_bytes(size_t value) { return sizeof(size_t); }
  static size_t get_required_bytes(int value) { return sizeof(int); }
  static size_t get_required_bytes(double value) { return sizeof(double); }

  /**
   * Appends the serialization of the provided bool value into the serialized
   * data buffer.
   * @param value The value to serialize into the serialized data buffer.
   * @return True if it successfully serialized the data. False if otherwise.
   */
  bool set_bool(bool value) {
    return this->set_generic(reinterpret_cast<unsigned char *>(&value),
        sizeof(value));
  }
  bool set_size_t(size_t value) {
    return this->set_generic(reinterpret_cast<unsigned char *>(&value),
        sizeof(value));
  }
  bool set_int(int value) {
    return this->set_generic(reinterpret_cast<unsigned char *>(&value),
        sizeof(value));
  }
  bool set_double(double value) {
    return this->set_generic(reinterpret_cast<unsigned char *>(&value),
        sizeof(value));
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
  bool set_generic(unsigned char *value, size_t num_bytes);

  /**
   * Returns the buffer containing the serialized data. Once this function
   * gets called, the caller is now the owner of the buffer.
   * @return The buffer containing the serialized data.
   */
  unsigned char *get_serialized_buffer();

  /**
   * Gets the number of bytes the serialized data takes up.
   * @return The size of the serialized data.
   */
  size_t get_size_serialized_data() { return this->offset; }

private:
  bool owns_buffer;
  unsigned char *buffer;
  size_t buffer_size;
  size_t offset;

  /**
   * A private helper function intended to grow the serialized buffer
   * quadratically. This function will only work if the serializer owns the
   * buffer.
   * @return True if it was able to grow the buffer. False if otherwise.
   */
  bool grow_buffer();
};

/**
 * Helper class that assists with deserializing data. It does not know what
 * data type or object is serialized within the buffer, as all data is binary.
 * Thus, the caller of the deserializer must know what format or type the
 * serialized data contains.
 */
class Deserializer {
public:
  /**
   * A copy constructor of a deserializer. It does not use the same buffer as
   * the buffer used in the provided deserializer, so it will own the buffer
   * the resulting deserializer holds. The deserializer will also start from
   * the beginning of the buffer when reading.
   * @param deserializer The deserializer to copy from.
   */
  Deserializer(Deserializer &deserializer)
      : Deserializer(deserializer.buffer, deserializer.num_bytes, false) {}

  /**
   * A constructor of a deserializer.
   * @param buffer The buffer to deserialize_as_message from.
   * @param num_bytes The number of bytes of the buffer.
   * @param steal True if this buffer be owned by the deserializer, or
   *        False if the buffer should be copied in order for the buffer
   *        remain external.
   */
  Deserializer(unsigned char *buffer, size_t num_bytes, bool steal);

  /**
   * Constructs a deserializer.
   * @param buffer The buffer to deserialize_as_message from. It will not be
   * modified by the deserializer, and the deserializer will not own the buffer.
   * @param num_bytes The number of bytes the buffer is.
   * @throws If the buffer provided is null, it will terminate the program.
   */
  Deserializer(unsigned char *buffer, size_t num_bytes);

  ~Deserializer();

  /**
   * Gets the number of bytes that are available to be read in the serialized
   * data.
   * @return The number of bytes that are available to be read.
   */
  size_t get_num_bytes_left();

  /**
   * Determines if there is enough bytes left to be read from the buffer in
   * order to retrieve a certain data value.
   * @return True if there is enough data left to read the specified data
   * value. False if otherwise.
   */
  bool has_bool() { return this->get_num_bytes_left() >= sizeof(bool); }
  bool has_size_t() { return this->get_num_bytes_left() >= sizeof(size_t); }
  bool has_int() { return this->get_num_bytes_left() >= sizeof(int); }
  bool has_double() { return this->get_num_bytes_left() >= sizeof(double); }

  /**
   * Gets the next value of buffer. It will move the pointer within the
   * buffer, so calling this function again will read the next byte of the
   * buffer.
   * @return The next value of the buffer at the specified type.
   * @throws Throws an error and terminates the program if there is not
   *        enough bytes left to be able to read from the buffer.
   */
  unsigned char get_byte();
  bool get_bool();
  size_t get_size_t();
  int get_int();
  double get_double();

  /**
   * Returns the buffer that the deserializer is pointing to.
   * @return Pointer to the buffer. The buffer is still owned by the
   *        deserializer, or whoever had created the deserializer. So this
   *        buffer should not be modified.
   */
  unsigned char *get_buffer() { return this->buffer; }

  /**
   * The number of bytes that the buffer is.
   * @return The size of the buffer.
   */
  size_t get_buffer_size() { return this->num_bytes; }

private:
  unsigned char *buffer; // Does not own
  size_t num_bytes;
  size_t offset;

  bool own_buffer;
};

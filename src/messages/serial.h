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

#include "custom_string.h"

class Ack;
class Nack;
class Put;
class Reply;
class Get;
class WaitAndGet;
class Status;
class Kill;
class Register;
class Directory;

enum class MsgKind {
  Ack,
  Nack,
  Put,
  Reply,
  Get,
  WaitAndGet,
  Status,
  Kill,
  Register,
  Directory
};

/**
 * An abstract class that details a serializeable and deserializeable message.
 * A serialized message will always begin with a header, which contains the
 * message kind, sender id, target id, and id. Then it will contain an
 * optional payload. If the sender id or the target id can be interpreted as
 * -1, then the sender/target is the rendezvous server.
 */
class Message : public Object {
public:
  static const size_t HEADER_SIZE = sizeof(MsgKind) + 4 * sizeof(size_t);

  MsgKind kind_;  // the message kind
  size_t sender_; // the index of the sender node
  size_t target_; // the index of the receiver node
  size_t id_;     // an id t unique within the node
  size_t size_;   // Size of the payload

  /**
   * Constructs a generic message.
   * @param kind The kind of message.
   */
  explicit Message(MsgKind kind) {
    this->kind_ = kind;
    this->sender_ = -1;
    this->target_ = -1;
    this->id_ = 0;
    this->size_ = 0;
  }

  /**
   * Gets the message type.
   * @return The message type.
   */
  MsgKind get_message_kind() { return this->kind_; }

  /**
   * Gets the sender id.
   * @return The sender id. If the id is -1, then it is from the server.
   */
  size_t get_sender_id() { return this->sender_; }

  /**
   * Gets the target id.
   * @return The target id. If the id is -1, then it is from the server.
   */
  size_t get_target_id() { return this->target_; }

  /**
   * Gets the id of the message.
   * @return The id of the message.
   */
  size_t get_id() { return this->id_; }

  /**
   * Gets the size of the message payload.
   * @return The size of the payload.
   */
  size_t get_payload_size() { return this->size_; }

  /**
   * Sets the sender id of the message.
   * @param sender The sender id of the message. If the id is -1, then it is
   *        from the server
   */
  void set_sender_id(size_t sender) { this->sender_ = sender; }

  /**
   * Sets the target id of the message.
   * @param target The target id of the message. If the id is -1, then the
   *        message is to the server.
   */
  void set_target_id(size_t target) { this->target_ = target; }

  /**
   * Sets the id of the message.
   * @param id The unique id of the message.
   */
  void set_id(size_t id) { this->id_ = id; }

  /**
   * Sets the payload of the message.
   * @param payload_size The payload of the message
   */
  void set_payload_size(size_t payload_size) { this->size_ = payload_size; }

  /**
   * Serializes the contents of the message into the provided buffer.
   * @param buffer_size The function will not do anything with the value.
   *        Instead, once the function completes, it will set the value with
   *        the size of the buffer containing the serialized message.
   * @return A pointer to the buffer containing the message in a serialized
   *        format. The buffer will then be owned by the caller.
   *        If it failed to serialize the data, it will return with nullptr.
   */
  virtual unsigned char *serialize(size_t &buffer_size) { assert(false); }

  /**
   * Takes the buffer provided and deserializes only the header of the
   * message into a new message instance. This is used for inspection
   * purposes ONLY, as it does not deserialize a full message.
   * @param buffer The buffer containing the message to deserialize.
   * @param num_bytes The number of bytes the buffer is.
   * @return A new instance of Message (cannot call any as_* functions) that
   * the caller will now acquire. If it failed to deserialize, it will return
   * with nullptr.
   */
  static Message *deserialize_header(unsigned char *buffer, size_t num_bytes);

  /**
   * Takes the buffer provided and deserializes it into a new message instance.
   * @param buffer The buffer containing the message to deserialize.
   * @param num_bytes The number of bytes the message is.
   * @return A new instance of the message with its contents deserialized. If
   *        it failed to deserialize, it will return with nullptr.
   */
  static Message *deserialize(unsigned char *buffer, size_t num_bytes);

  /**
   * A helper function used by the children classes of message. It will
   * prepare a newly allocated buffer that can serialize the message into.
   * The buffer will be of the requested number of bytes + the number of
   * bytes that would contain the header (message kind, sender id, target id,
   * and id). The buffer will have the header already filled in.
   * @param requested_bytes The number of bytes the buffer should be,
   *        excluding the space needed for the header.
   * @param offset A space of memory to hold the offset. The function will
   *        not use the offset at first, but after the function has finished,
   *        the offset value will be updated to the offset from the pointer of
   *        the returned buffer where the buffer is blank (no serailized data)
   * @return The prepared buffer that a message can be serialized into. The
   *         prepared buffer will contain the header already set up.
   */
  unsigned char *prepare_serialize_buffer(size_t requested_bytes,
                                          size_t &offset) {
    unsigned char *ret_value =
        new unsigned char[requested_bytes + Message::HEADER_SIZE];

    // Fill in the header
    MsgKind *casted_kind_buffer = reinterpret_cast<MsgKind *>(ret_value);
    casted_kind_buffer[0] = this->kind_;
    size_t *casted_size_buffer =
        reinterpret_cast<size_t *>(ret_value + sizeof(MsgKind));
    casted_size_buffer[0] = this->sender_;
    casted_size_buffer[1] = this->target_;
    casted_size_buffer[2] = this->id_;
    casted_size_buffer[3] = requested_bytes;

    offset = Message::HEADER_SIZE;
    return ret_value;
  }

  /**
   * Serializes the provided size_t value into the buffer starting from the
   * offset value.
   * @param size The value to put into the buffer.
   * @param buffer The buffer to serialize the value into.
   * @param offset The offset, in bytes, from the beginning address of the
   *        buffer. This will be the offset where the value should be stored
   *        in the buffer.
   * @param buffer_size The maximum size of the buffer.
   * @return The offset value from the beginning address of the buffer to the
   *        beginning of the blank portion of the buffer in bytes.
   * @throws If the buffer provided is not big enough to store the provided
   *         string, the program will be terminated. If the provided buffer
   *         is null, the program will be terminated. If the buffer and the
   *         buffer size are not reflective of the allocated memory, the
   *         behavior is undefined.
   */
  static size_t set_size_t_serialization(size_t size, unsigned char *buffer,
                                         size_t offset, size_t buffer_size) {
    assert(buffer != nullptr);
    assert(offset <= buffer_size);

    // Get the amount of bytes required in order set the provided string as a
    // serialized value in the buffer
    size_t required_bytes = sizeof(size_t);

    // Check if the buffer has enough space to write the string
    assert(buffer_size - offset >= required_bytes);

    // Fill in the size value for the string at the first index
    size_t *size_t_buffer = reinterpret_cast<size_t *>(buffer + offset);
    size_t_buffer[0] = size;

    // Return the new offset after the string has been added.
    return offset + required_bytes;
  }

  /**
   * Gets the size_t value stored in the buffer by the provided offset.
   * @param buffer The buffer to deserialize from.
   * @param offset The offset from the start of the buffer to read the value
   *        from. After the function returns, this value will be updated to
   *        the offset
   * @param buffer_size
   * @return The value pulled from the buffer.
   */
  static size_t get_size_t_deserialization(unsigned char *buffer,
                                           size_t &offset, size_t buffer_size) {
    // Make sure that the buffer has enough room to read at least a size of
    // the string
    assert(buffer != nullptr);
    assert(offset <= buffer_size);
    assert(buffer_size - offset >= sizeof(size_t));

    // Get the size of the string
    size_t *size_t_buffer = reinterpret_cast<size_t *>(buffer + offset);
    size_t ret_value = size_t_buffer[0];

    // Update the offset to a new offset based off the string
    offset += sizeof(size_t);

    return ret_value;
  }

  /**
   * Gets the required bytes needed to store the provided value in memory.
   * @param value The value that should be stored later.
   * @return The nubmer of bytes required to store the provided value in memory.
   */
  static size_t get_required_bytes(size_t value) { return sizeof(value); }

  /**
   * Serializes the provided string value into the buffer starting from the
   * offset value.
   * @param str The value to store. the string will remain external.
   * @param buffer The buffer to serialize the value into.
   * @param offset The offset, in bytes, from the beginning address of the
   *        buffer. This will be the offset where the value should be stored
   *        in the buffer.
   * @param buffer_size The maximum size of the buffer.
   * @return The offset value from the beginning address of the buffer to the
   *        beginning of the blank portion of the buffer in bytes.
   * @throws If the buffer provided is not big enough to store the provided
   *         string, the program will be terminated. If the provided buffer
   *         is null, the program will be terminated. If the buffer and the
   *         buffer size are not reflective of the allocated memory, the
   *         behavior is undefined.
   */
  static size_t set_string_serialization(String *str, unsigned char *buffer,
                                         size_t offset, size_t buffer_size) {
    assert(buffer != nullptr);
    assert(offset <= buffer_size);
    assert(str != nullptr);

    // Get the amount of bytes required in order set the provided string as a
    // serialized value in the buffer
    size_t required_bytes = sizeof(size_t) + (sizeof(char) * (str->size() + 1));

    // Check if the buffer has enough space to write the string
    assert(buffer_size - offset >= required_bytes);

    // Fill in the size value for the string at the first index
    size_t *size_t_buffer = reinterpret_cast<size_t *>(buffer + offset);
    size_t_buffer[0] = str->size();

    // Now fill in the actual string + the null terminator
    char *char_buffer =
        reinterpret_cast<char *>(buffer + offset + sizeof(size_t));
    for (size_t i = 0; i < str->size(); i++) {
      char_buffer[i] = str->at(i);
    }
    char_buffer[str->size()] = '\0'; // Tack on null terminator

    // Return the new offset after the string has been added.
    return offset + required_bytes;
  }

  /**
   * Gets the string value stored in the buffer by the provided offset.
   * @param buffer The buffer to deserialize from.
   * @param offset The offset from the start of the buffer to read the value
   *        from. After the function returns, this value will be updated to
   *        the offset
   * @param buffer_size The maximum size of the buffer.
   * @return The value pulled from the buffer. The string will be owned by
   *        the caller.
   */
  static String *get_string_deserialization(unsigned char *buffer,
                                            size_t &offset,
                                            size_t buffer_size) {
    // Make sure that the buffer has enough room to read at least a size of
    // the string
    assert(buffer != nullptr);
    assert(offset <= buffer_size);
    assert(buffer_size - offset >= sizeof(size_t));

    // Get the size of the string
    size_t *size_t_buffer = reinterpret_cast<size_t *>(buffer + offset);
    size_t size_of_string = size_t_buffer[0];

    // Make sure that the buffer has enough room to read the promised string
    assert(buffer_size - offset - sizeof(size_t) >= size_of_string + 1);

    // Get the starting pointer of the c string
    const char *c_str =
        reinterpret_cast<const char *>(buffer + offset + sizeof(size_t));

    // Create the string
    String *ret_value = new String(c_str, size_of_string);

    // Update the offset to a new offset based off the string
    offset += sizeof(size_t) + size_of_string + 1;

    return ret_value;
  }

  /**
   * Gets the required bytes needed to store the provided value in memory.
   * @param value The value that should be stored later.
   * @return The nubmer of bytes required to store the provided value in memory.
   */
  static size_t get_required_bytes(String *value) {
    assert(value != nullptr);
    return sizeof(size_t) + value->size() + 1;
  }

  /**
   * Serializes the provided int value into the buffer starting from the
   * offset value.
   * @param value The value to put into the buffer.
   * @param buffer The buffer to serialize the value into.
   * @param offset The offset, in bytes, from the beginning address of the
   *        buffer. This will be the offset where the value should be stored
   *        in the buffer.
   * @param buffer_size The maximum size of the buffer.
   * @return The offset value from the beginning address of the buffer to the
   *        beginning of the blank portion of the buffer in bytes.
   * @throws If the buffer provided is not big enough to store the provided
   *         string, the program will be terminated. If the provided buffer
   *         is null, the program will be terminated. If the buffer and the
   *         buffer size are not reflective of the allocated memory, the
   *         behavior is undefined.
   */
  static size_t set_int_serialization(int value, unsigned char *buffer,
                                      size_t offset, size_t buffer_size) {
    assert(buffer != nullptr);
    assert(offset <= buffer_size);

    // Get the amount of bytes required in order set the provided string as a
    // serialized value in the buffer
    size_t required_bytes = sizeof(int);

    // Check if the buffer has enough space to write the string
    assert(buffer_size - offset >= required_bytes);

    // Fill in the size value for the string at the first index
    int *casted_buffer = reinterpret_cast<int *>(buffer + offset);
    casted_buffer[0] = value;

    // Return the new offset after the string has been added.
    return offset + required_bytes;
  }

  /**
   * Gets the int value stored in the buffer by the provided offset.
   * @param buffer The buffer to deserialize from.
   * @param offset The offset from the start of the buffer to read the value
   *        from. After the function returns, this value will be updated to
   *        the offset
   * @param buffer_size The maximum size of the buffer.
   * @return The value pulled from the buffer.
   */
  static int get_int_deserialization(unsigned char *buffer, size_t &offset,
                                     size_t buffer_size) {
    // Make sure that the buffer has enough room to read at least a size of
    // the string
    assert(buffer != nullptr);
    assert(offset <= buffer_size);
    assert(buffer_size - offset >= sizeof(int));

    // Get the size of the string
    int *casted_buffer = reinterpret_cast<int *>(buffer + offset);
    int ret_value = casted_buffer[0];

    // Update the offset to a new offset based off the string
    offset += sizeof(int);

    return ret_value;
  }

  /**
   * Gets the required bytes needed to store the provided value in memory.
   * @param value The value that should be stored later.
   * @return The nubmer of bytes required to store the provided value in memory.
   */
  static size_t get_required_bytes(int value) { return sizeof(value); }

  /**
   * Serializes the provided double value into the buffer starting from the
   * offset value.
   * @param value The value to put into the buffer.
   * @param buffer The buffer to serialize the value into.
   * @param offset The offset, in bytes, from the beginning address of the
   *        buffer. This will be the offset where the value should be stored
   *        in the buffer.
   * @param buffer_size The maximum size of the buffer.
   * @return The offset value from the beginning address of the buffer to the
   *        beginning of the blank portion of the buffer in bytes.
   * @throws If the buffer provided is not big enough to store the provided
   *         string, the program will be terminated. If the provided buffer
   *         is null, the program will be terminated. If the buffer and the
   *         buffer size are not reflective of the allocated memory, the
   *         behavior is undefined.
   */
  static size_t set_double_serialization(double value, unsigned char *buffer,
                                         size_t offset, size_t buffer_size) {
    assert(buffer != nullptr);
    assert(offset <= buffer_size);

    // Get the amount of bytes required in order set the provided string as a
    // serialized value in the buffer
    size_t required_bytes = sizeof(double);

    // Check if the buffer has enough space to write the string
    assert(buffer_size - offset >= required_bytes);

    // Fill in the size value for the string at the first index
    double *casted_buffer = reinterpret_cast<double *>(buffer + offset);
    casted_buffer[0] = value;

    // Return the new offset after the string has been added.
    return offset + required_bytes;
  }

  /**
   * Gets the double value stored in the buffer by the provided offset.
   * @param buffer The buffer to deserialize from.
   * @param offset The offset from the start of the buffer to read the value
   *        from. After the function returns, this value will be updated to
   *        the offset
   * @param buffer_size The maximum size of the buffer.
   * @return The value pulled from the buffer.
   */
  static double get_double_deserialization(unsigned char *buffer,
                                           size_t &offset, size_t buffer_size) {
    // Make sure that the buffer has enough room to read at least a size of
    // the string
    assert(buffer != nullptr);
    assert(offset <= buffer_size);
    assert(buffer_size - offset >= sizeof(double));

    // Get the size of the string
    double *casted_buffer = reinterpret_cast<double *>(buffer + offset);
    double ret_value = casted_buffer[0];

    // Update the offset to a new offset based off the string
    offset += sizeof(double);

    return ret_value;
  }

  /**
   * Gets the required bytes needed to store the provided value in memory.
   * @param value The value that should be stored later.
   * @return The nubmer of bytes required to store the provided value in memory.
   */
  static size_t get_required_bytes(double value) { return sizeof(value); }

  /**
   * Serializes the provided bool value into the buffer starting from the
   * offset value.
   * @param value The value to put into the buffer.
   * @param buffer The buffer to serialize the value into.
   * @param offset The offset, in bytes, from the beginning address of the
   *        buffer. This will be the offset where the value should be stored
   *        in the buffer.
   * @param buffer_size The maximum size of the buffer.
   * @return The offset value from the beginning address of the buffer to the
   *        beginning of the blank portion of the buffer in bytes.
   * @throws If the buffer provided is not big enough to store the provided
   *         string, the program will be terminated. If the provided buffer
   *         is null, the program will be terminated. If the buffer and the
   *         buffer size are not reflective of the allocated memory, the
   *         behavior is undefined.
   */
  size_t set_bool_serialization(bool value, unsigned char *buffer,
                                size_t offset, size_t buffer_size) {
    assert(buffer != nullptr);
    assert(offset <= buffer_size);

    // Get the amount of bytes required in order set the provided string as a
    // serialized value in the buffer
    size_t required_bytes = sizeof(bool);

    // Check if the buffer has enough space to write the string
    assert(buffer_size - offset >= required_bytes);

    // Fill in the size value for the string at the first index
    bool *casted_buffer = reinterpret_cast<bool *>(buffer + offset);
    casted_buffer[0] = value;

    // Return the new offset after the string has been added.
    return offset + required_bytes;
  }

  /**
   * Gets the bool value stored in the buffer by the provided offset.
   * @param buffer The buffer to deserialize from.
   * @param offset The offset from the start of the buffer to read the value
   *        from. After the function returns, this value will be updated to
   *        the offset
   * @param buffer_size The maximum size of the buffer.
   * @return The value pulled from the buffer.
   */
  bool get_bool_deserialization(unsigned char *buffer, size_t &offset,
                                size_t buffer_size) {
    // Make sure that the buffer has enough room to read at least a size of
    // the string
    assert(buffer != nullptr);
    assert(offset <= buffer_size);
    assert(buffer_size - offset >= sizeof(bool));

    // Get the size of the string
    bool *casted_buffer = reinterpret_cast<bool *>(buffer + offset);
    bool ret_value = casted_buffer[0];

    // Update the offset to a new offset based off the string
    offset += sizeof(bool);

    return ret_value;
  }

  /**
   * Gets the required bytes needed to store the provided value in memory.
   * @param value The value that should be stored later.
   * @return The nubmer of bytes required to store the provided value in memory.
   */
  static size_t get_required_bytes(bool value) { return sizeof(value); }

  /**
   * Casts the message to an Ack message, if the message is an Ack message.
   * @return The casted message, or nullptr if the message is not of the
   *        correct type
   */
  virtual Ack *as_ack() { return nullptr; }

  /**
   * Casts the message to an Nack message, if the message is an Nack message.
   * @return The casted message, or nullptr if the message is not of the
   *        correct type
   */
  virtual Nack *as_nack() { return nullptr; }

  /**
   * Casts the message to an Put message, if the message is an Put message.
   * @return The casted message, or nullptr if the message is not of the
   *        correct type
   */
  virtual Put *as_put() { return nullptr; }

  /**
   * Casts the message to an Reply message, if the message is an Reply message.
   * @return The casted message, or nullptr if the message is not of the
   *        correct type
   */
  virtual Reply *as_reply() { return nullptr; }

  /**
   * Casts the message to an Get message, if the message is an Get message.
   * @return The casted message, or nullptr if the message is not of the
   *        correct type
   */
  virtual Get *as_get() { return nullptr; }

  /**
   * Casts the message to an WaitAndGet message, if the message is an WaitAndGet
   * message.
   * @return The casted message, or nullptr if the message is not of the
   *        correct type
   */
  virtual WaitAndGet *as_waitandget() { return nullptr; }

  /**
   * Casts the message to an Status message, if the message is an Status
   * message.
   * @return The casted message, or nullptr if the message is not of the
   *        correct type
   */
  virtual Status *as_status() { return nullptr; }

  /**
   * Casts the message to an Kill message, if the message is an Kill message.
   * @return The casted message, or nullptr if the message is not of the
   *        correct type
   */
  virtual Kill *as_kill() { return nullptr; }

  /**
   * Casts the message to an Register message, if the message is an Register
   * message.
   * @return The casted message, or nullptr if the message is not of the
   *        correct type
   */
  virtual Register *as_register() { return nullptr; }

  /**
   * Casts the message to an Directory message, if the message is an Directory
   * message.
   * @return The casted message, or nullptr if the message is not of the
   *        correct type
   */
  virtual Directory *as_directory() { return nullptr; }

  bool equals(Object *other) override {
    if (other == this)
      return true;
    Message *x = dynamic_cast<Message *>(other);
    if (x == nullptr)
      return false;
    return ((this->kind_ == x->kind_) && (this->sender_ == x->sender_) &&
            (this->target_ == x->target_) && (this->id_ == x->id_));
  }

  size_t hash_me() override {
    return static_cast<size_t>(this->kind_) + this->sender_ + this->target_ +
           this->id_;
  }
};

class Ack : public Message {
public:
  /**
   * Constructs an Ack message.
   */
  Ack() : Message(MsgKind::Ack) {}

  /**
   * Constructs a Ack message from a serialized buffer payload through
   * deserialization.
   * @param buffer A pointer to the start of the payload containing the
   *        status message.
   * @param num_bytes The number of bytes the payload is.
   * @throws If the provided payload does not have the expected size, it will
   *         terminate the program. A payload not of the correct message type
   *         will result in undefined behavior.
   */
  Ack(unsigned char *payload, size_t num_bytes) : Message(MsgKind::Ack) {
    // There is no payload
    assert(num_bytes == 0);
  }

  unsigned char *serialize(size_t &buffer_size) override {
    // Just prepare the message header since it has no payload.
    return this->prepare_serialize_buffer(0, buffer_size);
  }

  Ack *as_ack() override { return this; }
};

class Nack : public Message {
public:
  /**
   * Constructs a Nack message.
   */
  Nack() : Message(MsgKind::Nack) {}

  /**
   * Constructs a Nack message from a serialized buffer payload through
   * deserialization.
   * @param buffer A pointer to the start of the payload containing the
   *        status message.
   * @param num_bytes The number of bytes the payload is.
   * @throws If the provided payload does not have the expected size, it will
   *         terminate the program. A payload not of the correct message type
   *         will result in undefined behavior.
   */
  Nack(unsigned char *payload, size_t num_bytes) : Message(MsgKind::Nack) {
    // There is no payload
    assert(num_bytes == 0);
  }

  unsigned char *serialize(size_t &buffer_size) override {
    // Just prepare the message header since it has no payload.
    return this->prepare_serialize_buffer(0, buffer_size);
  }

  Nack *as_nack() override { return this; }
};

class Put : public Message {
public:
  /**
   * Constructs a Put message.
   */
  Put() : Message(MsgKind::Put) {}

  /**
   * Constructs a Put message from a serialized buffer payload through
   * deserialization
   * @param buffer A pointer to the start of the payload containing the
   *        status message.
   * @param num_bytes The number of bytes the payload is.
   * @throws If the provided payload does not have the expected size, it will
   *         terminate the program. A payload not of the correct message type
   *         will result in undefined behavior.
   */
  Put(unsigned char *payload, size_t num_bytes) : Message(MsgKind::Put) {
    // There is no payload
    assert(num_bytes == 0);
  }

  unsigned char *serialize(size_t &buffer_size) override {
    // Just prepare the message header since it has no payload.
    return this->prepare_serialize_buffer(0, buffer_size);
  }

  Put *as_put() override { return this; }
};

class Reply : public Message {
public:
  /**
   * Constructs a Reply message.
   */
  Reply() : Message(MsgKind::Reply) {}

  /**
   * Constructs a Reply message from a serialized buffer payload through
   * deserialization.
   * @param buffer A pointer to the start of the payload containing the
   *        status message.
   * @param num_bytes The number of bytes the payload is.
   * @throws If the provided payload does not have the expected size, it will
   *         terminate the program. A payload not of the correct message type
   *         will result in undefined behavior.
   */
  Reply(unsigned char *payload, size_t num_bytes) : Message(MsgKind::Reply) {
    // There is no payload
    assert(num_bytes == 0);
  }

  unsigned char *serialize(size_t &buffer_size) override {
    // Just prepare the message header since it has no payload.
    return this->prepare_serialize_buffer(0, buffer_size);
  }

  Reply *as_reply() override { return this; }
};

class Get : public Message {
public:
  /**
   * Constructs a Get message.
   */
  Get() : Message(MsgKind::Get) {}

  /**
   * Constructs a Get message from a serialized buffer payload through
   * deserialization.
   * @param buffer A pointer to the start of the payload containing the
   *        status message.
   * @param num_bytes The number of bytes the payload is.
   * @throws If the provided payload does not have the expected size, it will
   *         terminate the program. A payload not of the correct message type
   *         will result in undefined behavior.
   */
  Get(unsigned char *payload, size_t num_bytes) : Message(MsgKind::Get) {
    // There is no payload
    assert(num_bytes == 0);
  }

  unsigned char *serialize(size_t &buffer_size) override {
    // Just prepare the message header since it has no payload.
    return this->prepare_serialize_buffer(0, buffer_size);
  }

  Get *as_get() override { return this; }
};

class WaitAndGet : public Message {
public:
  /**
   * Constructs a WaitAndGet message.
   */
  WaitAndGet() : Message(MsgKind::WaitAndGet) {}

  /**
   * Constructs a WaitAndGet message from a serialized buffer payload through
   * deserialization.
   * @param buffer A pointer to the start of the payload containing the
   *        status message.
   * @param num_bytes The number of bytes the payload is.
   * @throws If the provided payload does not have the expected size, it will
   *         terminate the program. A payload not of the correct message type
   *         will result in undefined behavior.
   */
  WaitAndGet(unsigned char *payload, size_t num_bytes)
      : Message(MsgKind::WaitAndGet) {
    // There is no payload
    assert(num_bytes == 0);
  }

  unsigned char *serialize(size_t &buffer_size) override {
    // Just prepare the message header since it has no payload.
    return this->prepare_serialize_buffer(0, buffer_size);
  }

  WaitAndGet *as_waitandget() override { return this; }
};

class Status : public Message {
public:
  String *msg_; // owned

  /**
   * Constructs a Status message.
   * @param message The message that the status message will have. This
   * string will remain external.
   */
  explicit Status(String &message) : Message(MsgKind::Status) {
    this->msg_ = new String(message);
  }

  /**
   * Constructs a blank Status message.
   */
  Status() : Message(MsgKind::Status) { this->msg_ = new String(""); }

  /**
   * Constructs a Status message from a serialized buffer payload.
   * @param buffer A pointer to the start of the payload containing the
   *        status message.
   * @param num_bytes The number of bytes the payload is.
   * @throws If the provided payload does not have the expected size, it will
   *         terminate the program. A payload not of the correct message type
   *         will result in undefined behavior.
   */
  Status(unsigned char *payload, size_t num_bytes) : Message(MsgKind::Status) {
    // Pull the message from the payload
    size_t offset = 0;
    this->msg_ = this->get_string_deserialization(payload, offset, num_bytes);
  }

  /**
   * Deconstructs the message.
   */
  ~Status() override { delete this->msg_; }

  /**
   * Constructs a status message from a serialized buffer payload.
   * @param buffer A pointer to the start of the payload containing the
   *        status message.
   * @param num_bytes The number of bytes the payload is.
   * @throws If the provided payload does not have the expected size, it will
   *         terminate the program. A payload not of the correct message type
   *         will result in undefined behavior.
   */
  unsigned char *serialize(size_t &buffer_size) override {
    // Get the number of bytes we will need
    size_t required_bytes = Message::get_required_bytes(this->msg_);

    // Get the buffer ready
    size_t offset = 0;
    unsigned char *buffer =
        this->prepare_serialize_buffer(required_bytes, offset);
    buffer_size = required_bytes + offset;

    // Set the values into the buffer
    offset = Message::set_string_serialization(this->msg_, buffer, offset,
                                               buffer_size);

    return buffer;
  }

  /**
   * Sets the message of the status as the provided message. The argument
   * will remain external.
   * @param new_msg The new message.
   */
  void set_message(String &new_msg) {
    delete this->msg_;
    this->msg_ = new String(new_msg);
  }

  /**
   * Returns the message of the status.
   * @return The message of the status. Mutating the return value will not
   *        affect the Status message.
   */
  String *get_message() { return new String(*this->msg_); }

  Status *as_status() override { return this; }

  virtual bool equals(Object *other) override {
    if (other == this)
      return true;
    if (!Message::equals(other))
      return false;

    Status *x = dynamic_cast<Status *>(other);
    if (x == nullptr)
      return false;
    return this->msg_->equals(x->msg_);
  }

  virtual size_t hash_me() override {
    return Message::hash_me() + this->msg_->hash_me();
  }
};

class Kill : public Message {
public:
  /**
   * Constructs a Kill message.
   */
  Kill() : Message(MsgKind::Kill) {}

  /**
   * Constructs a Kill message from a serialized buffer payload through
   * deserialization.
   * @param buffer A pointer to the start of the payload containing the
   *        status message.
   * @param num_bytes The number of bytes the payload is.
   * @throws If the provided payload does not have the expected size, it will
   *         terminate the program. A payload not of the correct message type
   *         will result in undefined behavior.
   */
  Kill(unsigned char *payload, size_t num_bytes) : Message(MsgKind::Kill) {
    // There is no payload
    assert(num_bytes == 0);
  }

  unsigned char *serialize(size_t &buffer_size) override {
    // Just prepare the message header since it has no payload.
    return this->prepare_serialize_buffer(0, buffer_size);
  }

  Kill *as_kill() override { return this; }
};

class Register : public Message {
public:
  struct sockaddr_in client;
  size_t port;

  /**
   * Constructs a blank Register message.
   */
  Register() : Message(MsgKind::Register) {
    // Initialize the properties with default values
    this->client = {};
    this->client.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &this->client.sin_addr);
    this->client.sin_port = htons(-1);
    this->port = -1;
  }

  /**
   * Constructs a register message.
   * @param ip_addr The IP address the register message should have. The
   *        string will remain external.
   * @param port_num The port number the register message should have.
   * @throws If the provided payload does not have the expected size, it will
   *         terminate the program. A payload not of the correct message type
   *         will result in undefined behavior.
   */
  Register(String *ip_addr, int port_num) : Message(MsgKind::Register) {
    this->client = {};
    this->client.sin_family = AF_INET;
    inet_pton(AF_INET, ip_addr->c_str(), &this->client.sin_addr);
    this->client.sin_port = htons(port_num);

    this->port = static_cast<size_t>(port_num);
  }

  /**
   * Constructs a Register message from a serialized buffer payload through
   * deserialization.
   * @param buffer A pointer to the start of the payload containing the
   *        status message.
   * @param num_bytes The number of bytes the payload is.
   */
  Register(unsigned char *payload, size_t num_bytes)
      : Message(MsgKind::Register) {
    assert(payload != nullptr);

    // Get the number of bytes this message should be
    size_t required_bytes =
        Message::get_required_bytes(this->port) + sizeof(struct sockaddr_in);
    assert(num_bytes == required_bytes);

    // Get the values
    struct sockaddr_in *sock_addr_buffer =
        reinterpret_cast<struct sockaddr_in *>(payload);
    this->client = sock_addr_buffer[0];

    size_t offset = sizeof(struct sockaddr_in);
    this->port =
        Message::get_size_t_deserialization(payload, offset, num_bytes);
  }

  unsigned char *serialize(size_t &buffer_size) override {
    // Get the number of bytes we will need
    size_t required_bytes =
        sizeof(struct sockaddr_in) + Message::get_required_bytes(this->port);

    // Get the buffer ready
    size_t offset = 0;
    unsigned char *buffer =
        this->prepare_serialize_buffer(required_bytes, offset);
    buffer_size = offset + required_bytes;

    // Set the values into the buffer
    struct sockaddr_in *sock_addr_buffer =
        reinterpret_cast<struct sockaddr_in *>(buffer + offset);
    sock_addr_buffer[0] = this->client;
    offset += sizeof(struct sockaddr_in);
    offset = Message::set_size_t_serialization(this->port, buffer, offset,
                                               buffer_size);

    return buffer;
  }

  void set_ip_addr(String *ip_addr) {
    inet_pton(AF_INET, ip_addr->c_str(), &this->client.sin_addr);
  }

  void set_port_num(int port_num) { this->client.sin_port = htons(port_num); }

  /**
   * Gets the IP address of the message.
   * @return A new string containing the IP address. The returned value will
   *        now be owned by the caller.
   */
  String *get_ip_addr() {
    struct in_addr ip_addr = this->client.sin_addr;
    char addr_cstr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &ip_addr, addr_cstr, INET_ADDRSTRLEN);
    return new String(addr_cstr);
  }

  /**
   * Gets the port number.
   * @return The port number. -1 means that it has not been set yet.
   */
  int get_port_num() { return static_cast<int>(this->port); }

  Register *as_register() override { return this; }

  virtual bool equals(Object *other) override {
    if (other == this)
      return true;
    if (!Message::equals(other))
      return false;

    Register *x = dynamic_cast<Register *>(other);
    if (x == nullptr)
      return false;
    String *this_ip = this->get_ip_addr();
    String *that_ip = x->get_ip_addr();

    bool ret_value = ((this->client.sin_family == x->client.sin_family) &&
                      (this_ip->equals(that_ip)) && (this->port == x->port));

    delete this_ip;
    delete that_ip;

    return ret_value;
  }

  virtual size_t hash_me() override {
    size_t ret_value = Message::hash_me();
    ret_value += this->port;
    String *ip_addr = this->get_ip_addr();
    ret_value += ip_addr->hash_me();

    delete ip_addr;
    return ret_value;
  }
};

class Directory : public Message {
public:
  size_t clients;
  size_t *ports;      // owned
  String **addresses; // owned; strings owned

  /**
   * Constructs a blank directory message.
   * @param max_clients
   */
  explicit Directory(size_t max_clients) : Message(MsgKind::Directory) {
    assert(max_clients > 0);

    this->clients = max_clients;
    this->ports = new size_t[this->clients];
    this->addresses = new String *[this->clients];
    for (size_t i = 0; i < this->clients; i++) {
      this->ports[i] = 0;
      this->addresses[i] = nullptr;
    }
  }

  /**
   * Constructs a Directory message from a serialized buffer payload through
   * deserialization.
   * @param buffer A pointer to the start of the payload containing the
   *        status message.
   * @param num_bytes The number of bytes the payload is.
   * @throws If the provided payload does not have the expected size, it will
   *         terminate the program. A payload not of the correct message type
   *         will result in undefined behavior.
   */
  Directory(unsigned char *payload, size_t num_bytes)
      : Message(MsgKind::Directory) {
    assert(payload != nullptr);

    // Get the number of bytes this message should be at least
    size_t required_bytes = Message::get_required_bytes(this->clients);
    assert(num_bytes > required_bytes);

    // Get the maximum number of clients for the directory
    size_t offset = 0;
    this->clients =
        Message::get_size_t_deserialization(payload, offset, num_bytes);

    // Ensure that the directory message is big enough to hold all the
    // clients (including the blank ones)
    String *blank_string = new String("");
    required_bytes += this->clients * sizeof(size_t) +
                      this->clients * Message::get_required_bytes(blank_string);
    assert(num_bytes >= required_bytes);
    this->ports = new size_t[this->clients];
    this->addresses = new String *[this->clients];

    // Get the port and IP address information and store them accordingly
    for (size_t i = 0; i < this->clients; i++) {
      this->ports[i] =
          Message::get_size_t_deserialization(payload, offset, num_bytes);
    }
    for (size_t i = 0; i < this->clients; i++) {
      String *address =
          Message::get_string_deserialization(payload, offset, num_bytes);
      if (address->equals(blank_string)) {
        // Replace it with a nullptr
        this->addresses[i] = nullptr;
        delete address;
      } else {
        this->addresses[i] = address;
      }
    }

    delete blank_string;
  }

  /**
   * Deconstructs the message.
   */
  ~Directory() override {
    delete[] this->ports;
    for (size_t i = 0; i < this->clients; i++) {
      if (this->addresses[i] != nullptr) {
        delete this->addresses[i];
      }
    }
    delete[] this->addresses;
    this->clients = 0;
  }

  unsigned char *serialize(size_t &buffer_size) override {
    // Get the number of bytes we will need
    size_t testing = Message::HEADER_SIZE;
    size_t required_bytes = Message::get_required_bytes(clients);
    required_bytes +=
        Message::get_required_bytes(this->ports[0]) * this->clients;
    String *blank_string = new String("");
    for (size_t i = 0; i < this->clients; i++) {
      if (this->addresses[i] != nullptr) {
        required_bytes += Message::get_required_bytes(this->addresses[i]);
      } else {
        // Request enough bytes for an empty string as a place holder for
        // empty client ids
        required_bytes += Message::get_required_bytes(blank_string);
      }
    }

    // Get the buffer ready
    size_t offset = 0;
    unsigned char *buffer =
        this->prepare_serialize_buffer(required_bytes, offset);
    buffer_size = offset + required_bytes;

    // Set the values into the buffer
    offset = Message::set_size_t_serialization(this->clients, buffer, offset,
                                               buffer_size);
    for (size_t i = 0; i < this->clients; i++) {
      offset = Message::set_size_t_serialization(this->ports[i], buffer, offset,
                                                 buffer_size);
    }
    for (size_t i = 0; i < this->clients; i++) {
      if (this->addresses[i] != nullptr) {
        offset = Message::set_string_serialization(this->addresses[i], buffer,
                                                   offset, buffer_size);
      } else {
        // Request enough bytes for an empty string as a place holder for
        // empty client ids
        offset = Message::set_string_serialization(blank_string, buffer, offset,
                                                   buffer_size);
      }
    }

    delete blank_string;
    return buffer;
  }

  /**
   * Adds a client to the directory.
   * @param client_id The id of the client to be added. The id will be from 0
   *        to 1 less than the maximum number of clients that the registrar
   *        server can have registered at any given one time.
   * @param ip_addr The IP address of the client. It will remain external.
   * @param True if the client could be added. False if it could not, either
   *        because the id is invalid, because there is already a client
   *        registered at that id, or if the ip address provided is null.
   */
  bool add_client(size_t client_id, String *ip_addr, int port_num) {
    assert((client_id >= 0) && (client_id < this->clients));
    assert(ip_addr != nullptr);

    if (this->addresses[client_id] == nullptr) {
      this->addresses[client_id] = new String(*ip_addr);
      this->ports[client_id] = static_cast<size_t>(port_num);
      return true;
    } else {
      return false;
    }
  }

  /**
   * Removes a client from the directory.
   * @param client_id The id of the client to be removed. The id will be from 0
   *        to 1 less tha the maximum number of clients taht the registrar
   *        server can have registered at any given one time.
   * @param True if the client had been removed. False if there is no client
   *        registered at the id. If the client id is invalid, it will
   *        termiante the program.
   */
  bool remove_client(size_t client_id) {
    assert((client_id >= 0) && (client_id < this->clients));

    if (this->addresses[client_id] == nullptr) {
      return false;
    } else {
      delete this->addresses[client_id];
      this->addresses[client_id] = nullptr;
      this->ports[client_id] = 0;
      return true;
    }
  }

  /**
   * Gets the maximum number of clients that can be registered at any time.
   * @return The maximum number of clients that can be registered at the same
   *        time.
   */
  size_t get_max_num_clients() { return this->clients; }

  /**
   * Determines if there is a client registered at the specified index.
   * @param client_id The id of the client. The id will be from 0 to 1 less than
   *        the maximum number of clients that the register server can have
   *        registered at any given one time.
   * @return True if there is a client connected at this id. False if otherwise.
   */
  bool is_client_connected(size_t client_id) {
    assert((client_id >= 0) && (client_id < this->clients));
    return (this->addresses[client_id] != nullptr);
  }

  /**
   * Gets the ip address of the client.
   * @param client_id The id of the client. The id will be from 0 to 1 less
   *        than the maximum number of clients that the registrar server can
   *        have registered at any given one time.
   * @return The IP address of the client. The string will be owned by the
   *        caller. It will return nullptr if there is not client registered
   *        at that id. If the id provided is invalid, it will termiante the
   *        program.
   */
  String *get_client_ip_addr(size_t client_id) {
    assert((client_id >= 0) && (client_id < this->clients));

    if (this->addresses[client_id] == nullptr) {
      return nullptr;
    } else {
      return new String(*this->addresses[client_id]);
    }
  }

  /**
   * Gets the port number of the client.
   * @param client_id The id of the client. The id will be from 0 to 1 less
   *        than the maximum number of clients that the registrar server can
   *        have registered at anuy given one time.
   * @return The port number of the client. It will return 0 if there is no
   *        client registered at that id. If the id provided is invalid, it
   *        will terminate the program.
   */
  int get_client_port_num(size_t client_id) {
    assert((client_id >= 0) && (client_id < this->clients));
    return this->ports[client_id];
  }

  Directory *as_directory() override { return this; }

  virtual bool equals(Object *other) override {
    if (other == this)
      return true;
    if (!Message::equals(other))
      return false;

    Directory *x = dynamic_cast<Directory *>(other);
    if (x == nullptr)
      return false;
    if (this->clients != x->clients)
      return false;
    for (size_t i = 0; i < this->clients; i++) {
      if (this->ports[i] != x->ports[i])
        return false;

      if ((this->addresses[i] != nullptr) && (x->addresses[i] != nullptr)) {
        if (!this->addresses[i]->equals(x->addresses[i]))
          return false;
      } else if ((this->addresses[i] == nullptr) &&
                 (x->addresses[i] != nullptr)) {
        return false;
      } else if ((this->addresses[i] != nullptr) &&
                 (x->addresses[i] == nullptr)) {
        return false;
      }
    }

    return true;
  }

  virtual size_t hash_me() override {
    size_t ret_value = Message::hash_me();
    ret_value += this->clients;
    for (size_t i = 0; i < this->clients; i++) {
      ret_value += this->ports[i];
      ret_value += this->addresses[i]->hash_me();
    }
    return ret_value;
  }
};

Message *Message::deserialize_header(unsigned char *buffer, size_t num_bytes) {
  if (num_bytes < Message::HEADER_SIZE) {
    // A valid message buffer should always have a header. A message
    // smaller than that is invalid.
    return nullptr;
  }

  MsgKind *casted_msgkind_buffer = reinterpret_cast<MsgKind *>(buffer);
  size_t *casted_header_buffer =
      reinterpret_cast<size_t *>(buffer + sizeof(MsgKind));
  MsgKind type = casted_msgkind_buffer[0];
  size_t sender = casted_header_buffer[0];
  size_t target = casted_header_buffer[1];
  size_t id = casted_header_buffer[2];
  size_t len = casted_header_buffer[3];

  Message *ret_value = new Message(type);
  ret_value->set_sender_id(sender);
  ret_value->set_target_id(target);
  ret_value->set_id(id);
  ret_value->set_payload_size(len);
  return ret_value;
}

Message *Message::deserialize(unsigned char *buffer, size_t num_bytes) {
  if (num_bytes < Message::HEADER_SIZE) {
    // A valid message buffer should always have a header. A message
    // smaller than that is invalid.
    return nullptr;
  }

  // Grab the header by casting the buffer to the correct values.
  MsgKind *casted_msgkind_buffer = reinterpret_cast<MsgKind *>(buffer);
  size_t *casted_header_buffer =
      reinterpret_cast<size_t *>(buffer + sizeof(MsgKind));
  MsgKind type = casted_msgkind_buffer[0];
  size_t sender = casted_header_buffer[0];
  size_t target = casted_header_buffer[1];
  size_t id = casted_header_buffer[2];
  size_t len = casted_header_buffer[3];

  // Get the beggining of the payload
  size_t payload_size = num_bytes - Message::HEADER_SIZE;
  unsigned char *payload = buffer + Message::HEADER_SIZE;

  Message *ret_value = nullptr;
  switch (type) {
  case MsgKind::Ack:
    ret_value = new Ack(payload, payload_size);
    break;
  case MsgKind::Nack:
    ret_value = new Nack(payload, payload_size);
    break;
  case MsgKind::Put:
    ret_value = new Put(payload, payload_size);
    break;
  case MsgKind::Reply:
    ret_value = new Reply(payload, payload_size);
    break;
  case MsgKind::Get:
    ret_value = new Get(payload, payload_size);
    break;
  case MsgKind::WaitAndGet:
    ret_value = new WaitAndGet(payload, payload_size);
    break;
  case MsgKind::Status:
    ret_value = new Status(payload, payload_size);
    break;
  case MsgKind::Kill:
    ret_value = new Kill(payload, payload_size);
    break;
  case MsgKind::Register:
    ret_value = new Register(payload, payload_size);
    break;
  case MsgKind::Directory:
    ret_value = new Directory(payload, payload_size);
    break;
  default:
    // Invalid message type.
    break;
  }

  if (ret_value != nullptr) {
    // Fill in the header if we returned with a created message
    ret_value->set_sender_id(sender);
    ret_value->set_target_id(target);
    ret_value->set_id(id);
    ret_value->set_payload_size(len);
  }

  return ret_value;
}

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
class Message : public CustomObject {
public:
  static const size_t HEADER_SIZE = sizeof(MsgKind) + 4 * sizeof(size_t);

  /**
   * Constructs a generic message.
   * @param kind The kind of message.
   */
  explicit Message(MsgKind kind);

  /**
   * Gets the message type.
   * @return The message type.
   */
  MsgKind get_message_kind() { return this->kind; }

  /**
   * Gets the sender id.
   * @return The sender id. If the id is -1, then it is from the server.
   */
  size_t get_sender_id() { return this->sender; }

  /**
   * Gets the target id.
   * @return The target id. If the id is -1, then it is from the server.
   */
  size_t get_target_id() { return this->target; }

  /**
   * Gets the id of the message.
   * @return The id of the message.
   */
  size_t get_id() { return this->id; }

  /**
   * Gets the size of the message payload.
   * @return The size of the payload.
   */
  size_t get_payload_size() { return this->size; }

  /**
   * Sets the sender id of the message.
   * @param sender_id The sender id of the message. If the id is -1, then it is
   *        from the server
   */
  void set_sender_id(size_t sender_id) { this->sender = sender_id; }

  /**
   * Sets the target id of the message.
   * @param target_id The target id of the message. If the id is -1, then the
   *        message is to the server.
   */
  void set_target_id(size_t target_id) { this->target = target_id; }

  /**
   * Sets the id of the message.
   * @param msg_id The unique id of the message.
   */
  void set_id(size_t msg_id) { this->id = msg_id; }

  /**
   * Sets the payload of the message.
   * @param payload_size The payload of the message
   */
  void set_payload_size(size_t payload_size) { this->size = payload_size; }

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

  bool equals(CustomObject *other) override;
  size_t hash_me() override;
  void serialize(Serializer &serializer) override;

  /**
   * Takes the buffer provided and deserializes only the header of the
   * message into a new message instance. This is used for inspection
   * purposes ONLY, as it does not deserialize_as_message a full message.
   * @param buffer The buffer containing the message to deserialize_as_message.
   * @param num_bytes The number of bytes the buffer is.
   * @return A new instance of Message (cannot call any as_* functions) that
   * the caller will now acquire. If it failed to deserialize_as_message, it
   * will return with nullptr.
   */
  static Message *deserialize_as_message_header(unsigned char *buffer,
                                                size_t num_bytes);

  /**
   * Takes the buffer provided and deserializes it into a new message instance.
   * @param buffer The buffer containing the message to deserialize_as_message.
   * @param num_bytes The number of bytes the message is.
   * @return A new instance of the message with its contents deserialized. If
   *        it failed to deserialize_as_message, it will return with nullptr.
   */
  static Message *deserialize_as_message(unsigned char *buffer,
                                         size_t num_bytes);

private:
  MsgKind kind;  // the message kind
  size_t sender; // the index of the sender node
  size_t target; // the index of the receiver node
  size_t id;     // an id t unique within the node
  size_t size;   // Size of the payload
};

class Ack : public Message {
public:
  /**
   * Constructs an Ack message.
   */
  Ack();

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
  Ack(unsigned char *payload, size_t num_bytes);

  void serialize(Serializer &serializer) override;
  Ack *as_ack() override { return this; }
};

class Nack : public Message {
public:
  /**
   * Constructs a Nack message.
   */
  Nack();

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
  Nack(unsigned char *payload, size_t num_bytes);

  void serialize(Serializer &serializer) override;
  Nack *as_nack() override { return this; }
};

/**
 * An abstract class implementing a message that employs
 * serializer/deserializers as the payload to facilitate more generic message
 * payloads.
 */
class SerializerMessage_ : public Message {
public:
  /**
   * Constructs a serializer message with the specified deserializer. The
   * class will now own the provided deserializer.
   * @param type The message type.
   * @param deserializer The deserializer to be used as the payload for the
   *        message.
   */
  SerializerMessage_(MsgKind type, Deserializer *deserializer);

  /**
   * Constructs a message from a serialized buffer payload through
   * deserialization
   * @param buffer A pointer to the start of the payload containing the
   *        status message.
   * @param num_bytes The number of bytes the payload is.
   * @throws If the provided payload does not have the expected size, it will
   *         terminate the program. A payload not of the correct message type
   *         will result in undefined behavior.
   */
  SerializerMessage_(MsgKind type, unsigned char *payload, size_t num_bytes);

  /**
   * Constructs a message with a serializer.
   * @param serializer Construct a put message with a serializer. This
   *        serializer will be used afterwards, as it will take the resulting
   *        buffer.
   */
  explicit SerializerMessage_(MsgKind type, Serializer &serializer);

  /**
   * Deconstructs the put message.
   */
  ~SerializerMessage_() override;

  void serialize(Serializer &serializer) override;

  /**
   * Gets the deserializer associated with the message's payload. Once this
   * function gets called, the deserializer is no longer owned by this
   * object, and instead is owned by the caller.
   * @return The deserializer. Nullptr if there is no payload associated with
   * the message.
   */
  Deserializer *steal_deserializer();

private:
  Deserializer *deserializer;
  bool owned = true;
};

class Put : public SerializerMessage_ {
public:
  Put() : SerializerMessage_(MsgKind::Put, nullptr) {}

  Put(unsigned char *payload, size_t num_bytes)
      : SerializerMessage_(MsgKind::Put, payload, num_bytes) {}

  explicit Put(Serializer &serializer)
      : SerializerMessage_(MsgKind::Put, serializer) {}

  Put *as_put() override { return this; }
};

class Reply : public SerializerMessage_ {
public:
  Reply() : SerializerMessage_(MsgKind::Reply, nullptr) {}

  Reply(unsigned char *payload, size_t num_bytes)
      : SerializerMessage_(MsgKind::Reply, payload, num_bytes) {}

  explicit Reply(Serializer &serializer)
      : SerializerMessage_(MsgKind::Reply, serializer) {}

  Reply *as_reply() override { return this; }
};

class Get : public SerializerMessage_ {
public:
  Get() : SerializerMessage_(MsgKind::Get, nullptr) {}

  Get(unsigned char *payload, size_t num_bytes)
      : SerializerMessage_(MsgKind::Get, payload, num_bytes) {}

  explicit Get(Serializer &serializer)
      : SerializerMessage_(MsgKind::Get, serializer) {}

  Get *as_get() override { return this; }
};

class WaitAndGet : public SerializerMessage_ {
public:
  WaitAndGet() : SerializerMessage_(MsgKind::WaitAndGet, nullptr) {}

  WaitAndGet(unsigned char *payload, size_t num_bytes)
      : SerializerMessage_(MsgKind::WaitAndGet, payload, num_bytes) {}

  explicit WaitAndGet(Serializer &serializer)
      : SerializerMessage_(MsgKind::WaitAndGet, serializer) {}

  WaitAndGet *as_waitandget() override { return this; }
};

class Status : public Message {
public:
  /**
   * Constructs a Status message.
   * @param message The message that the status message will have. This
   * string will remain external.
   */
  explicit Status(String &message);

  /**
   * Constructs a blank Status message.
   */
  Status();

  /**
   * Constructs a Status message from a serialized buffer payload.
   * @param buffer A pointer to the start of the payload containing the
   *        status message.
   * @param num_bytes The number of bytes the payload is.
   * @throws If the provided payload does not have the expected size, it will
   *         terminate the program. A payload not of the correct message type
   *         will result in undefined behavior.
   */
  Status(unsigned char *payload, size_t num_bytes);

  /**
   * Deconstructs the message.
   */
  ~Status() override;

  void serialize(Serializer &serializer) override;

  /**
   * Sets the message of the status as the provided message. The argument
   * will remain external.
   * @param new_msg The new message.
   */
  void set_message(String &new_msg);

  /**
   * Returns the message of the status.
   * @return The message of the status. Mutating the return value will not
   *        affect the Status message.
   */
  String *get_message();

  Status *as_status() override { return this; }

  bool equals(CustomObject *other) override;
  size_t hash_me() override;

private:
  String *msg; // owned
};

class Kill : public Message {
public:
  /**
   * Constructs a Kill message.
   */
  Kill();

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
  Kill(unsigned char *payload, size_t num_bytes);

  void serialize(Serializer &serializer) override;

  Kill *as_kill() override { return this; }
};

class Register : public Message {
public:
  /**
   * Constructs a blank Register message.
   */
  Register();

  /**
   * Constructs a register message.
   * @param ip_addr The IP address the register message should have. The
   *        string will remain external.
   * @param port_num The port number the register message should have.
   * @throws If the provided payload does not have the expected size, it will
   *         terminate the program. A payload not of the correct message type
   *         will result in undefined behavior.
   */
  Register(String *ip_addr, int port_num);

  /**
   * Constructs a Register message from a serialized buffer payload through
   * deserialization.
   * @param buffer A pointer to the start of the payload containing the
   *        status message.
   * @param num_bytes The number of bytes the payload is.
   */
  Register(unsigned char *payload, size_t num_bytes);

  void serialize(Serializer &serializer) override;

  void set_ip_addr(String *ip_addr);
  void set_port_num(int port_num);

  /**
   * Gets the IP address of the message.
   * @return A new string containing the IP address. The returned value will
   *        now be owned by the caller.
   */
  String *get_ip_addr();

  /**
   * Gets the port number.
   * @return The port number. -1 means that it has not been set yet.
   */
  int get_port_num();

  Register *as_register() override { return this; }

  bool equals(CustomObject *other) override;
  size_t hash_me() override;

private:
  struct sockaddr_in client{};
  size_t port;
};

class Directory : public Message {
public:
  /**
   * Constructs a blank directory message.
   * @param max_clients
   */
  explicit Directory(size_t max_clients);

  /**
   * Copy constructor.
   * @param director Directory to copy from
   */
  Directory(Directory &directory);

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
  Directory(unsigned char *payload, size_t num_bytes);

  /**
   * Deconstructs the message.
   */
  ~Directory() override;

  void serialize(Serializer &serializer) override;

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
  bool add_client(size_t client_id, String *ip_addr, int port_num);

  /**
   * Removes a client from the directory.
   * @param client_id The id of the client to be removed. The id will be from 0
   *        to 1 less tha the maximum number of clients taht the registrar
   *        server can have registered at any given one time.
   * @param True if the client had been removed. False if there is no client
   *        registered at the id. If the client id is invalid, it will
   *        termiante the program.
   */
  bool remove_client(size_t client_id);

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
  bool is_client_connected(size_t client_id);

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
  String *get_client_ip_addr(size_t client_id);

  /**
   * Gets the port number of the client.
   * @param client_id The id of the client. The id will be from 0 to 1 less
   *        than the maximum number of clients that the registrar server can
   *        have registered at anuy given one time.
   * @return The port number of the client. It will return 0 if there is no
   *        client registered at that id. If the id provided is invalid, it
   *        will terminate the program.
   */
  int get_client_port_num(size_t client_id);

  Directory *as_directory() override { return this; }
  bool equals(CustomObject *other) override;
  size_t hash_me() override;

private:
  size_t clients;
  size_t *ports;      // owned
  String **addresses; // owned; strings owned
};

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

class Put;
class Reply;
class WaitAndGet;
class Status;
class Register;
class Directory;

enum class MsgKind {
  Put,
  Reply,
  WaitAndGet,
  Status,
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

  explicit Message(MsgKind kind);

  MsgKind get_message_kind() { return this->kind; }
  size_t get_sender_id() { return this->sender; }
  size_t get_target_id() { return this->target; }
  size_t get_id() { return this->id; }
  size_t get_payload_size() { return this->size; }

  void set_sender_id(size_t sender_id) { this->sender = sender_id; }
  void set_target_id(size_t target_id) { this->target = target_id; }
  void set_id(size_t msg_id) { this->id = msg_id; }
  void set_payload_size(size_t payload_size) { this->size = payload_size; }

  /**
   * Casts the message to a specified message type.
   * @return The casted message, or nullptr if the message is not of the
   *        correct type
   */
  virtual Put *as_put() { return nullptr; }
  virtual Reply *as_reply() { return nullptr; }
  virtual WaitAndGet *as_waitandget() { return nullptr; }
  virtual Status *as_status() { return nullptr; }
  virtual Register *as_register() { return nullptr; }
  virtual Directory *as_directory() { return nullptr; }

  bool equals(CustomObject *other) const override;
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
  explicit Status(String &message);
  Status();
  Status(unsigned char *payload, size_t num_bytes);
  ~Status() override;

  void serialize(Serializer &serializer) override;

  void set_message(String &new_msg);
  String *get_message();

  Status *as_status() override { return this; }

  bool equals(CustomObject *other) const override;
  size_t hash_me() override;

private:
  String *msg; // owned
};

class Register : public Message {
public:
  Register();
  Register(String *ip_addr, int port_num);
  Register(unsigned char *payload, size_t num_bytes);

  void serialize(Serializer &serializer) override;

  String *get_ip_addr() const;
  int get_port_num() const;

  Register *as_register() override { return this; }

  bool equals(CustomObject *other) const override;
  size_t hash_me() override;

private:
  struct sockaddr_in client{};
  size_t port;
};

class Directory : public Message {
public:
  explicit Directory(size_t max_clients);
  Directory(unsigned char *payload, size_t num_bytes);
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

  size_t get_max_num_clients() { return this->clients; }
  bool is_client_connected(size_t client_id);
  String *get_client_ip_addr(size_t client_id);
  int get_client_port_num(size_t client_id);

  Directory *as_directory() override { return this; }
  bool equals(CustomObject *other) const override;
  size_t hash_me() override;

private:
  size_t clients;
  size_t *ports;      // owned
  String **addresses; // owned; strings owned
};

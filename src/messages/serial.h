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
  // TODO: Create a serializer helper class to assist serialization
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

  void serialize(Serializer &serializer) override {
    // Serialize the header.
    // NOTE: All Message classes should call this function before serializing
    // the rest of their payloads!
    serializer.set_generic(&this->kind_, sizeof(MsgKind));
    serializer.set_size_t(this->sender_);
    serializer.set_size_t(this->target_);
    serializer.set_size_t(this->id_);
    serializer.set_size_t(this->size_);
  }

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

  bool equals(CustomObject *other) override {
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

  void serialize(Serializer &serializer) override {
    // Just prepare the message header since it has no payload
    this->set_payload_size(0);
    return Message::serialize(serializer);
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

  void serialize(Serializer &serializer) override {
    // Just prepare the message header since it has no payload
    this->set_payload_size(0);
    return Message::serialize(serializer);
  }

  Nack *as_nack() override { return this; }
};

/**
 * An abstract class implementing a message that employs
 * serializer/deserializers as the payload to facilitate more generic message
 * payloads.
 */
class SerializerMessage_ : public Message {
public:
  Deserializer *deserializer_;
  bool owned_ = true;

  /**
   * Constructs a serializer message with the specified deserializer. The
   * class will now own the provided deserializer.
   * @param type The message type.
   * @param deserializer The deserializer to be used as the payload for the
   *        message.
   */
  SerializerMessage_(MsgKind type, Deserializer *deserializer) : Message(type) {
    this->deserializer_ = deserializer;
  }

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
  SerializerMessage_(MsgKind type, unsigned char *payload, size_t num_bytes)
      : Message(type) {
    this->deserializer_ = new Deserializer(payload, num_bytes, false);
  }

  /**
   * Constructs a message with a serializer.
   * @param serializer Construct a put message with a serializer. This
   *        serializer will be used afterwards, as it will take the resulting
   *        buffer.
   */
  explicit SerializerMessage_(MsgKind type, Serializer &serializer)
    : Message(type) {
    this->deserializer_ =
        new Deserializer(serializer.get_serialized_buffer(),
                         serializer.get_size_serialized_data(), true);
  }

  /**
   * Deconstructs the put message.
   */
  ~SerializerMessage_() override {
    if (this->owned_ && this->deserializer_ != nullptr) {
      delete this->deserializer_;
    }
  }

  void serialize(Serializer &serializer) override {
    if (this->deserializer_ == nullptr) {
      // Just prepare the message header since it has no payload
      this->set_payload_size(0);
      return Message::serialize(serializer);
    } else {
      this->set_payload_size(this->deserializer_->get_buffer_size());
      Message::serialize(serializer);
      serializer.set_generic(this->deserializer_->get_buffer(),
                             this->deserializer_->get_buffer_size());
    }
  }

  /**
   * Gets the deserializer associated with the message's payload. Once this
   * function gets called, the deserializer is no longer owned by this
   * object, and instead is owned by the caller.
   * @return The deserializer. Nullptr if there is no payload associated with
   * the message.
   */
  Deserializer *steal_deserializer() {
    this->owned_ = false;
    return this->deserializer_;
  }
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
    Deserializer deserializer(payload, num_bytes);
    this->msg_ = String::deserialize_as_string(deserializer);
  }

  /**
   * Deconstructs the message.
   */
  ~Status() override { delete this->msg_; }

  void serialize(Serializer &serializer) override {
    // Prepare the header
    size_t payload_size = this->msg_->serialization_required_bytes();
    this->set_payload_size(payload_size);
    Message::serialize(serializer);

    // Now serialize it as a string
    this->msg_->serialize(serializer);
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

  virtual bool equals(CustomObject *other) override {
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

  void serialize(Serializer &serializer) override {
    // Just prepare the message header since it has no payload
    this->set_payload_size(0);
    return Message::serialize(serializer);
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
    Deserializer deserializer(payload, num_bytes);

    // Get the socket address
    unsigned char *sock_addr_buffer =
        reinterpret_cast<unsigned char *>(&this->client);
    for (size_t i = 0; i < sizeof(struct sockaddr_in); i++) {
      sock_addr_buffer[i] = deserializer.get_byte();
    }

    // Now get the port
    this->port = deserializer.get_size_t();
  }

  void serialize(Serializer &serializer) override {
    // Prepare the header
    size_t payload_size =
        sizeof(this->client) + Serializer::get_required_bytes(this->port);
    this->set_payload_size(payload_size);
    Message::serialize(serializer);

    // Now copy in the socket address structure byte by byte
    serializer.set_generic(&this->client, sizeof(this->client));

    // Set the port
    serializer.set_size_t(this->port);
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

  virtual bool equals(CustomObject *other) override {
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
   * Copy constructor.
   * @param director Directory to copy from
   */
  Directory(Directory &directory) : Message(MsgKind::Directory) {
    this->clients = directory.clients;

    this->ports = new size_t[this->clients];
    for (size_t i = 0; i < this->clients; i++) {
      this->ports[i] = directory.ports[i];
    }

    // Get the addresses
    this->addresses = new String *[this->clients];
    for (size_t i = 0; i < this->clients; i++) {
      if (directory.addresses[i] != nullptr) {
        this->addresses[i] = new String(directory.addresses[i]->c_str());
      }
      else {
        this->addresses[i] = nullptr;
      }
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
    Deserializer deserializer(payload, num_bytes);
    String blank_string("");

    // Get the maximum number of clients
    this->clients = deserializer.get_size_t();

    // Get the ports
    this->ports = new size_t[this->clients];
    for (size_t i = 0; i < this->clients; i++) {
      this->ports[i] = deserializer.get_size_t();
    }

    // Get the addresses
    this->addresses = new String *[this->clients];
    for (size_t i = 0; i < this->clients; i++) {
      String *address = String::deserialize_as_string(deserializer);
      if (address->equals(&blank_string)) {
        // Replace it with a nullptr
        this->addresses[i] = nullptr;
        delete address;
      } else {
        this->addresses[i] = address;
      }
    }
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

  void serialize(Serializer &serializer) override {
    // Prepare the header
    String blank_string("");
    size_t payload_size = Serializer::get_required_bytes(this->clients);
    for (size_t i = 0; i < this->clients; i++) {
      payload_size += Serializer::get_required_bytes(this->ports[i]);
      if (this->addresses[i] != nullptr) {
        payload_size += this->addresses[i]->serialization_required_bytes();
      } else {
        // Request enough bytes for an empty string as a place holder for
        // empty client ids
        payload_size += blank_string.serialization_required_bytes();
      }
    }
    this->set_payload_size(payload_size);
    Message::serialize(serializer);

    // Now serialize the values.
    serializer.set_size_t(this->clients);
    for (size_t i = 0; i < this->clients; i++) {
      serializer.set_size_t(this->ports[i]);
    }
    for (size_t i = 0; i < this->clients; i++) {
      if (this->addresses[i] != nullptr) {
        this->addresses[i]->serialize(serializer);
      } else {
        // Request enough bytes for an empty string as a place holder for
        // empty client ids
        blank_string.serialize(serializer);
      }
    }
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

  virtual bool equals(CustomObject *other) override {
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

Message *Message::deserialize_as_message_header(unsigned char *buffer,
                                                size_t num_bytes) {
  if (num_bytes < Message::HEADER_SIZE) {
    // A valid message buffer should always have a header. A message
    // smaller than that is invalid.
    return nullptr;
  } else {
    // Begin deserializing the header
    Deserializer deserializer(buffer, num_bytes);

    // Build the MsgKind by pulling it byte by byte
    MsgKind type;
    unsigned char *type_bytes = reinterpret_cast<unsigned char *>(&type);
    for (size_t i = 0; i < sizeof(MsgKind); i++) {
      type_bytes[i] = deserializer.get_byte();
    }

    // Now get the rest of the header
    size_t sender = deserializer.get_size_t();
    size_t target = deserializer.get_size_t();
    size_t id = deserializer.get_size_t();
    size_t len = deserializer.get_size_t();

    // Create the return value
    Message *ret_value = new Message(type);
    ret_value->set_sender_id(sender);
    ret_value->set_target_id(target);
    ret_value->set_id(id);
    ret_value->set_payload_size(len);
    return ret_value;
  }
}

Message *Message::deserialize_as_message(unsigned char *buffer,
                                         size_t num_bytes) {
  if (num_bytes < Message::HEADER_SIZE) {
    // A valid message buffer should always have a header. A message
    // smaller than that is invalid.
    return nullptr;
  }

  // Grab the header by casting the buffer to the correct values.
  Message *header = Message::deserialize_as_message_header(buffer, num_bytes);

  // Get the begining of the payload
  size_t payload_size = num_bytes - Message::HEADER_SIZE;
  unsigned char *payload = buffer + Message::HEADER_SIZE;

  Message *ret_value = nullptr;
  switch (header->get_message_kind()) {
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
    // Fill in the header into the return value
    ret_value->set_sender_id(header->get_sender_id());
    ret_value->set_target_id(header->get_target_id());
    ret_value->set_id(header->get_id());
    ret_value->set_payload_size(header->get_payload_size());
  }

  return ret_value;
}

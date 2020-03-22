/**
 * Name: Snowy Chen, Joe Song
 * Date: 21 March 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::Cpp

#include "serial.h"

Message::Message(MsgKind kind) {
  this->kind = kind;
  this->sender = -1;
  this->target = -1;
  this->id = 0;
  this->size = 0;
}

bool Message::equals(CustomObject *other) {
  if (other == this)
    return true;
  auto *x = dynamic_cast<Message *>(other);
  if (x == nullptr)
    return false;
  return ((this->kind == x->kind) && (this->sender == x->sender) &&
          (this->target == x->target) && (this->id == x->id));
}

size_t Message::hash_me() {
  return static_cast<size_t>(this->kind) + this->sender + this->target +
         this->id;
}

void Message::serialize(Serializer &serializer) {
  // Serialize the header.
  // NOTE: All Message classes should call this function before serializing
  // the rest of their payloads!
  serializer.set_generic(reinterpret_cast<unsigned char *>(&this->kind),
      sizeof(MsgKind));
  serializer.set_size_t(this->sender);
  serializer.set_size_t(this->target);
  serializer.set_size_t(this->id);
  serializer.set_size_t(this->size);
}

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
    auto *type_bytes = reinterpret_cast<unsigned char *>(&type);
    for (size_t i = 0; i < sizeof(MsgKind); i++) {
      type_bytes[i] = deserializer.get_byte();
    }

    // Now get the rest of the header
    size_t sender = deserializer.get_size_t();
    size_t target = deserializer.get_size_t();
    size_t id = deserializer.get_size_t();
    size_t len = deserializer.get_size_t();

    // Create the return value
    auto *ret_value = new Message(type);
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

  // Free the memory as necessary
  delete header;

  return ret_value;
}

Ack::Ack() : Message(MsgKind::Ack) {}

Ack::Ack(unsigned char *payload, size_t num_bytes) : Message(MsgKind::Ack) {
  // There is no payload
  assert(num_bytes == 0);
}

void Ack::serialize(Serializer &serializer) {
  // Just prepare the message header since it has no payload
  this->set_payload_size(0);
  return Message::serialize(serializer);
}

Nack::Nack() : Message(MsgKind::Nack) {}

Nack::Nack(unsigned char *payload, size_t num_bytes) : Message(MsgKind::Nack) {
  // There is no payload
  assert(num_bytes == 0);
}

void Nack::serialize(Serializer &serializer) {
  // Just prepare the message header since it has no payload
  this->set_payload_size(0);
  return Message::serialize(serializer);
}

SerializerMessage_::SerializerMessage_(MsgKind type, Deserializer *deserializer)
    : Message(type) {
  this->deserializer = deserializer;
}

SerializerMessage_::SerializerMessage_(MsgKind type, unsigned char *payload,
                                       size_t num_bytes)
    : Message(type) {
  this->deserializer = new Deserializer(payload, num_bytes, false);
}

SerializerMessage_::SerializerMessage_(MsgKind type, Serializer &serializer)
    : Message(type) {
  this->deserializer =
      new Deserializer(serializer.get_serialized_buffer(),
                       serializer.get_size_serialized_data(), true);
}

SerializerMessage_::~SerializerMessage_() {
  if (this->owned && this->deserializer != nullptr) {
    delete this->deserializer;
  }
}

void SerializerMessage_::serialize(Serializer &serializer) {
  if (this->deserializer == nullptr) {
    // Just prepare the message header since it has no payload
    this->set_payload_size(0);
    return Message::serialize(serializer);
  } else {
    this->set_payload_size(this->deserializer->get_buffer_size());
    Message::serialize(serializer);
    serializer.set_generic(this->deserializer->get_buffer(),
                           this->deserializer->get_buffer_size());
  }
}

Deserializer *SerializerMessage_::steal_deserializer() {
  this->owned = false;
  return this->deserializer;
}

Status::Status(String &message) : Message(MsgKind::Status) {
  this->msg = new String(message);
}

Status::Status() : Message(MsgKind::Status) { this->msg = new String(""); }

Status::Status(unsigned char *payload, size_t num_bytes)
    : Message(MsgKind::Status) {
  // Pull the message from the payload
  Deserializer deserializer(payload, num_bytes);
  this->msg = String::deserialize_as_string(deserializer);
}

Status::~Status() { delete this->msg; }

void Status::serialize(Serializer &serializer) {
  // Prepare the header
  size_t payload_size = this->msg->serialization_required_bytes();
  this->set_payload_size(payload_size);
  Message::serialize(serializer);

  // Now serialize it as a string
  this->msg->serialize(serializer);
}

void Status::set_message(String &new_msg) {
  delete this->msg;
  this->msg = new String(new_msg);
}

String *Status::get_message() { return new String(*this->msg); }

bool Status::equals(CustomObject *other) {
  if (other == this)
    return true;
  if (!Message::equals(other))
    return false;

  auto *x = dynamic_cast<Status *>(other);
  if (x == nullptr)
    return false;
  return this->msg->equals(x->msg);
}

size_t Status::hash_me() { return Message::hash_me() + this->msg->hash_me(); }

Kill::Kill() : Message(MsgKind::Kill) {}

Kill::Kill(unsigned char *payload, size_t num_bytes) : Message(MsgKind::Kill) {
  // There is no payload
  assert(num_bytes == 0);
}

void Kill::serialize(Serializer &serializer) {
  // Just prepare the message header since it has no payload
  this->set_payload_size(0);
  return Message::serialize(serializer);
}

Register::Register() : Message(MsgKind::Register) {
  // Initialize the properties with default values
  this->client = {0};
  this->client.sin_family = AF_INET;
  inet_pton(AF_INET, "127.0.0.1", &this->client.sin_addr);
  this->client.sin_port = htons(-1);
  this->port = -1;
}

Register::Register(String *ip_addr, int port_num) : Message(MsgKind::Register) {
  this->client = {0};
  this->client.sin_family = AF_INET;
  inet_pton(AF_INET, ip_addr->c_str(), &this->client.sin_addr);
  this->client.sin_port = htons(port_num);

  this->port = static_cast<size_t>(port_num);
}

Register::Register(unsigned char *payload, size_t num_bytes)
    : Message(MsgKind::Register) {
  Deserializer deserializer(payload, num_bytes);

  // Get the socket address
  auto *sock_addr_buffer =
      reinterpret_cast<unsigned char *>(&this->client);
  for (size_t i = 0; i < sizeof(struct sockaddr_in); i++) {
    sock_addr_buffer[i] = deserializer.get_byte();
  }

  // Now get the port
  this->port = deserializer.get_size_t();
}

void Register::serialize(Serializer &serializer) {
  // Prepare the header
  size_t payload_size =
      sizeof(this->client) + Serializer::get_required_bytes(this->port);
  this->set_payload_size(payload_size);
  Message::serialize(serializer);

  // Now copy in the socket address structure byte by byte
  serializer.set_generic(reinterpret_cast<unsigned char *>(&this->client),
      sizeof(this->client));

  // Set the port
  serializer.set_size_t(this->port);
}

void Register::set_ip_addr(String *ip_addr) {
  inet_pton(AF_INET, ip_addr->c_str(), &this->client.sin_addr);
}

void Register::set_port_num(int port_num) {
  this->client.sin_port = htons(port_num);
}

String *Register::get_ip_addr() {
  struct in_addr ip_addr = this->client.sin_addr;
  char addr_cstr[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &ip_addr, addr_cstr, INET_ADDRSTRLEN);
  return new String(addr_cstr);
}

int Register::get_port_num() { return static_cast<int>(this->port); }

bool Register::equals(CustomObject *other) {
  if (other == this)
    return true;
  if (!Message::equals(other))
    return false;

  auto *x = dynamic_cast<Register *>(other);
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

size_t Register::hash_me() {
  size_t ret_value = Message::hash_me();
  ret_value += this->port;
  String *ip_addr = this->get_ip_addr();
  ret_value += ip_addr->hash_me();

  delete ip_addr;
  return ret_value;
}

Directory::Directory(size_t max_clients) : Message(MsgKind::Directory) {
  assert(max_clients > 0);

  this->clients = max_clients;
  this->ports = new size_t[this->clients];
  this->addresses = new String *[this->clients];
  for (size_t i = 0; i < this->clients; i++) {
    this->ports[i] = 0;
    this->addresses[i] = nullptr;
  }
}

Directory::Directory(Directory &directory) : Message(MsgKind::Directory) {
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
    } else {
      this->addresses[i] = nullptr;
    }
  }
}

Directory::Directory(unsigned char *payload, size_t num_bytes)
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

Directory::~Directory() {
  delete[] this->ports;
  for (size_t i = 0; i < this->clients; i++) {
    if (this->addresses[i] != nullptr) {
      delete this->addresses[i];
    }
  }
  delete[] this->addresses;
  this->clients = 0;
}

void Directory::serialize(Serializer &serializer) {
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

bool Directory::add_client(size_t client_id, String *ip_addr, int port_num) {
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

bool Directory::remove_client(size_t client_id) {
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

bool Directory::is_client_connected(size_t client_id) {
  assert((client_id >= 0) && (client_id < this->clients));
  return (this->addresses[client_id] != nullptr);
}

String *Directory::get_client_ip_addr(size_t client_id) {
  assert((client_id >= 0) && (client_id < this->clients));

  if (this->addresses[client_id] == nullptr) {
    return nullptr;
  } else {
    return new String(*this->addresses[client_id]);
  }
}

int Directory::get_client_port_num(size_t client_id) {
  assert((client_id >= 0) && (client_id < this->clients));
  return this->ports[client_id];
}

bool Directory::equals(CustomObject *other) {
  if (other == this)
    return true;
  if (!Message::equals(other))
    return false;

  auto *x = dynamic_cast<Directory *>(other);
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

size_t Directory::hash_me() {
  size_t ret_value = Message::hash_me();
  ret_value += this->clients;
  for (size_t i = 0; i < this->clients; i++) {
    ret_value += this->ports[i];
    ret_value += this->addresses[i]->hash_me();
  }
  return ret_value;
}

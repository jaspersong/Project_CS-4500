/**
 * Name: Snowy Chen, Joe Song
 * Date: 21 March 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::Cpp

#include "distributed_app.h"

class MessageReceiver {
public:
  static const size_t STARTING_BUFFER_SIZE = 1024;

  unsigned char *buffer;
  size_t buffer_index;
  size_t buffer_size;

  MessageReceiver() {
    this->buffer = new unsigned char[STARTING_BUFFER_SIZE];
    this->buffer_index = 0;
    this->buffer_size = STARTING_BUFFER_SIZE;
  }

  ~MessageReceiver() { delete[] this->buffer; }

  /**
   * Takes in a serial data chunk and determines if it constructs a full
   * message.
   * @param receive The buffer containing the received serialized buffer.
   * This buffer will remain external.
   * @param num_bytes The number of bytes of the data.
   * @return Nullptr if the data has not yet constructed a full message. It
   * will return a message if it constructs a full message, and the caller
   * will acquire the memory.
   */
  Message *receive_data(const unsigned char *receive, size_t num_bytes) {
    // Added the provided data chunk into the buffer we are building.
    Message *ret_value = nullptr;

    // TODO: Check for errors when building the message, such as being out of
    //  sync from the message stream
    if (this->buffer_index + num_bytes > this->buffer_size) {
      // Resize quadratically
      this->buffer_size = this->buffer_size * this->buffer_size;
      auto *new_buffer = new unsigned char[this->buffer_size];
      for (size_t i = 0; i < this->buffer_index; i++) {
        new_buffer[i] = this->buffer[i];
      }
      delete[] this->buffer;
      this->buffer = new_buffer;
    }
    for (size_t i = 0; i < num_bytes; i++) {
      this->buffer[i + this->buffer_index] = receive[i];
    }
    this->buffer_index += num_bytes;

    // Peek at the header for inspection
    Message *header = Message::deserialize_as_message_header(
        this->buffer, this->buffer_index);
    // Determine if it's a full message if the collected buffer is big enough
    // to hold the data that the message header promises
    if (header != nullptr) {
      if (header->get_payload_size() ==
          this->buffer_index - Message::HEADER_SIZE) {
        ret_value =
            Message::deserialize_as_message(this->buffer, this->buffer_index);
        this->buffer_index = 0;
      } else if (header->get_payload_size() <
                 this->buffer_index - Message::HEADER_SIZE) {
        ret_value =
            Message::deserialize_as_message(this->buffer, this->buffer_index);

        // Move the left over to the beginning
        size_t prev_message_size =
            header->get_payload_size() - Message::HEADER_SIZE;
        for (size_t i = 0; i < this->buffer_index - prev_message_size; i++) {
          this->buffer[i] = this->buffer[i + prev_message_size];
        }
        this->buffer_index = this->buffer_index - prev_message_size;
      }
    }

    delete header;
    return ret_value;
  }
};

/**
 * A helper class used to manage the direct connections to other clients
 * between Nodes.
 */
class Node::ClientDMManager : public Server {
public:
  Directory *directory;
  Lock *directory_lock;

  MessageReceiver *dm_msg_receiver;
  ReceivedMessageManager *dm_recv_msg_manager;

  ClientDMManager(String *ip_addr, int port_num, size_t max_clients,
                   ReceivedMessageManager &received_msg_manager)
      : Server(ip_addr, port_num, max_clients, 1024) {
    this->directory = new Directory(max_clients);
    this->directory_lock = new Lock();

    this->dm_msg_receiver = new MessageReceiver();
    this->dm_recv_msg_manager = &received_msg_manager;
    this->dm_recv_msg_manager->set_server(this);
  }

  ~ClientDMManager() override {
    this->close_server();
    delete this->directory;
    delete this->directory_lock;
  }

  void init() override {
    // Nothing special required
  }

  void handle_shutdown() override {
    // NOthing special required
  }

  void handle_incoming_message(size_t client_id, unsigned char *buffer,
                               size_t num_bytes) override {
    // Deserialize the data
    Message *message = this->dm_msg_receiver->receive_data(buffer, num_bytes);
    if (message == nullptr) {
      return;
    }
    switch (message->get_message_kind()) {
    case MsgKind::Put: {
      if (!this->dm_recv_msg_manager->handle_put(message->as_put())) {
        delete message;
      }
      break;
    }
    case MsgKind::Reply: {
      if (!this->dm_recv_msg_manager->handle_reply(message->as_reply())) {
        delete message;
      }
      break;
    }
    case MsgKind::WaitAndGet: {
      if (!this->dm_recv_msg_manager->handle_waitandget(message->as_waitandget())) {
        delete message;
      }
      break;
    }
    case MsgKind::Status: {
      if (!this->dm_recv_msg_manager->handle_status(message->as_status())) {
        delete message;
      }
      break;
    }
    case MsgKind::Register: {
      printf("Received Register message from client id %zu. Ignoring "
             "command.\n",
             client_id);
      delete message;
      break;
    }
    case MsgKind::Directory: {
      printf("Received Directory message from client id %zu. Ignoring "
             "command.\n",
             client_id);
      delete message;
      break;
    }
    default: {
      // Invalid message type. Do nothing and just ignore it.
      printf("Received invalid message from client id %zu\n", client_id);
      delete message;
      break;
    }
    }
  }

  void handle_incoming_connection(size_t new_client_id, String *addr,
                                  int port_num) override {
    // Nothing special is required when establishing a new connection to a
    // new client.
    printf("New client id %zu initiated connection\n", new_client_id);
  }

  void handle_closing_connection(size_t client_id) override {
    // Remove the client at that id
    printf("Client id %zu closed connection\n", client_id);
  }

  /**
   * Initiates a direct message connection to the specified client id.
   * @param client_id The unique client id.
   * @return True if it was able to successfully connect to the spcified
   * client, or if it was already connected. False if otherwise.
   */
  bool initiate_direct_message_connection(size_t client_id) {
    // Nothing to do if the client is already connected
    if (!this->is_client_connected(client_id)) {
      if (this->directory->is_client_connected(client_id)) {
        String *ip_addr = this->directory->get_client_ip_addr(client_id);
        int port_num = this->directory->get_client_port_num(client_id);
        bool ret_value =
            this->initiate_connection(client_id, ip_addr, port_num);

        delete ip_addr;
        return ret_value;
      } else {
        return false;
      }
    } else {
      return true;
    }
  }

  /**
   * Closes a direct message connection to the specified client at the client
   * id.
   * @param client_id The unique client id.
   */
  void close_direct_message_connection(size_t client_id) {
    // Nothing to do if the client is not connected
    if (this->is_client_connected(client_id)) {
      this->close_connection(client_id);
    }
  }

  /**
   * Sends a direct message to the client at the specified client id.
   * @param client_id The unique client id.
   * @param msg The message that will be sent to the specified client.
   * @return True if the message has successfully been queued. False if
   * otherwise.
   */
  bool send_direct_message(size_t client_id, Message &msg) {
    // Call the serializer to serialize the message.
    Serializer serialized_message;
    msg.serialize(serialized_message);
    unsigned char *message = serialized_message.get_serialized_buffer();
    size_t message_size = serialized_message.get_size_serialized_data();

    // Now sent it.
    return this->send_message(client_id, message, message_size);
  }

  /**
   * Ensures that the client at the specified id is directly connected. If
   * the client id is not registered yet, then it will return false.
   * @param client_id Unique client id that this client should be connecting
   * to.
   * @return True if the clients are directly connected. False if otherwise.
   */
  bool ensure_client_directly_connected(size_t client_id) {
    // We expect the client to be connected if it's available in the directory
    if (this->directory->is_client_connected(client_id)) {
      // Wait until the client has been connected
      if (!this->is_client_connected(client_id)) {
        Thread::sleep(1000);
      }
      return true;
    }
    else {
      return false;
    }
  }

  /**
   * Updates the directory for the direct message manager.
   * @param new_directory The updated directory. The function will acquire
   * ownership of this data.
   */
  void update_directory(Directory *new_directory) {
    this->directory_lock->lock();
    // Close sockets for clients that no longer are connected
    for (size_t i = 0; i < new_directory->get_max_num_clients(); i++) {
      if (!new_directory->is_client_connected(i)) {
        this->close_direct_message_connection(i);
      }
    }
    delete this->directory;
    this->directory = new_directory;
    this->directory_lock->unlock();
  }
};

Registrar::Registrar(String *ip_addr, int port_num, size_t max_num_clients,
                     ReceivedMessageManager &received_message_manager)
    : Server(ip_addr, port_num, max_num_clients,
             Server::DEFAULT_MAX_RECEIVE_SIZE) {
  // Initialize the client directory
  this->client_dir = new Directory(max_num_clients);
  // Set the sender to the server id
  this->client_dir->set_sender_id(-1);

  this->message_receiver = new MessageReceiver();
  this->received_msg_manager = &received_message_manager;
  this->received_msg_manager->set_server(this);
}

Registrar::~Registrar() {
  // Close the server
  this->close_server();
  while (this->running()) {
  }

  // Free the appropriate memory
  delete this->client_dir;
}

void Registrar::handle_incoming_message(size_t client_id, unsigned char *buffer,
                                        size_t num_bytes) {
  // Deserialize the data
  Message *message = this->message_receiver->receive_data(buffer, num_bytes);
  if (message == nullptr) {
    // Received incomplete message. Wait for it.
    return;
  }
  switch (message->get_message_kind()) {
  case MsgKind::Put: {
    if (!this->received_msg_manager->handle_put(message->as_put())) {
      delete message;
    }
    break;
  }
  case MsgKind::Reply: {
    if (!this->received_msg_manager->handle_reply(message->as_reply())) {
      delete message;
    }
    break;
  }
  case MsgKind::WaitAndGet: {
    if (!this->received_msg_manager->handle_waitandget(message->as_waitandget())) {
      delete message;
    }
    break;
  }
  case MsgKind::Status: {
    if (!this->received_msg_manager->handle_status(message->as_status())) {
      delete message;
    }
    break;
  }
  case MsgKind::Register: {
    // Add this client to the directory
    printf("Received Register message from client id %zu\n", client_id);

    // Register the client id to the directory
    Register *reg_message = message->as_register();
    String *ip_addr = reg_message->get_ip_addr();
    printf("Registered IP address %s, port number %d\n", ip_addr->c_str(),
           reg_message->get_port_num());
    this->client_dir->add_client(client_id, ip_addr,
                                  reg_message->get_port_num());

    // Now send out the updated directory to all of the other clients that
    // are connected.
    this->broadcast_client_directory();

    delete ip_addr;
    delete message;
    break;
  }
  case MsgKind::Directory: {
    // Do nothing with a directory message because this is the registration
    // server. The server should be the only one sending these sort of
    // messages.
    printf("Received Directory message from client id %zu. Ignoring "
           "command\n",
           client_id);
    delete message;
    break;
  }
  default: {
    // Invalid message type. Do nothing and just ignore it.
    printf("Received invalid message from client id %zu. Ignoring.\n",
           client_id);
    delete message;
    break;
  }
  }
}

void Registrar::handle_incoming_connection(size_t new_client_id, String *addr,
                                           int port_num) {
  (void)addr;
  (void)port_num;

  // Nothing special is required when establishing a new connection to a
  // new client.
  printf("New client id %zu initiated connection\n", new_client_id);
}

void Registrar::handle_closing_connection(size_t client_id) {
  // Remove the client at that id
  printf("Client id %zu closed connection\n", client_id);
  this->client_dir->remove_client(client_id);

  this->broadcast_client_directory();
}

void Registrar::broadcast_client_directory() {
  for (size_t i = 0; i < this->client_dir->get_max_num_clients(); i++) {
    if (this->client_dir->is_client_connected(i)) {
      this->client_dir->set_target_id(i);

      // Serialize the directory
      Serializer serialized_broadcast;
      this->client_dir->serialize(serialized_broadcast);
      unsigned char *broadcast = serialized_broadcast.get_serialized_buffer();
      size_t broadcast_size = serialized_broadcast.get_size_serialized_data();

      // Send the data
      this->send_message(i, broadcast, broadcast_size);
      printf("Sent directory message, size %zu to client id %zu\n",
             broadcast_size, i);
    }
  }
}

Node::Node(String *server_ip_addr, int server_port_num, String *ip_addr,
           int port_num, size_t fixed_num_clients,
           ReceivedMessageManager &msg_manager,
           ReceivedMessageManager &dm_msg_manager)
    : Client(server_ip_addr, server_port_num, fixed_num_clients) {
  this->dm_manager = new ClientDMManager(ip_addr, port_num, fixed_num_clients,
                                           dm_msg_manager);

  this->message_receiver = new MessageReceiver();
  this->received_msg_manager = &msg_manager;
  this->received_msg_manager->set_client(this);

  this->id = -1; // Default id for now until the registrar gives us an id
}

Node::~Node() {
  // Close the client
  this->close_client();
  this->dm_manager->close_server();
  delete this->dm_manager;
}

void Node::init() {
  // Start up the direct message manager
  this->dm_manager->start();

  // Set up the register message
  Register reg(this->dm_manager->get_ip_addr(), this->dm_manager->get_port_num());
  reg.set_sender_id(this->id);
  reg.set_target_id(-1); // Goes to the server

  // Serialize the message
  Serializer serialized_message;
  reg.serialize(serialized_message);
  unsigned char *msg = serialized_message.get_serialized_buffer();
  size_t num_bytes = serialized_message.get_size_serialized_data();

  this->send_message(msg, num_bytes);
}

void Node::handle_shutdown() { this->dm_manager->close_server(); }

void Node::handle_incoming_message(unsigned char *buffer, size_t num_bytes) {
  // Deserialize the data
  Message *message = this->message_receiver->receive_data(buffer, num_bytes);
  if (message == nullptr) {
    return;
  }
  switch (message->get_message_kind()) {
  case MsgKind::Put: {
    if (!this->received_msg_manager->handle_put(message->as_put())) {
      delete message;
    }
    break;
  }
  case MsgKind::Reply: {
    if (!this->received_msg_manager->handle_reply(message->as_reply())) {
      delete message;
    }
    break;
  }
  case MsgKind::WaitAndGet: {
    if (!this->received_msg_manager->handle_waitandget(message->as_waitandget())) {
      delete message;
    }
    break;
  }
  case MsgKind::Status: {
    if (!this->received_msg_manager->handle_status(message->as_status())) {
      delete message;
    }
    break;
  }
  case MsgKind::Register: {
    printf("Received a register message from the server. Ignoring message"
           ".\n");
    delete message;
    break;
  }
  case MsgKind::Directory: {
    // Replace the current directory with the updated directory
    printf("Received Directory from server\n");
    Directory *dir_message = message->as_directory();
    this->dm_manager->update_directory(dir_message);

    // Pull the node id from the directory message as the enumeration, if
    // this is the first directory message we have received.
    if (this->id == -1) { // This means that the node id hasn't been assigned.
      this->id = message->get_target_id();
      printf("Assigned node id %zu.\n", this->id);
      this->enum_signal.notify_all();
    }
    else {
      // Initiate the connection to all of the currently connected clients to
      // the directory. We can iterate from this id (exclusive) to
      // the last id because we know that the registrar will assign node ids in
      // the order that connect to the registrar
      for (size_t i = this->id + 1; dir_message->is_client_connected(i); i++) {
        while (!this->dm_manager->initiate_direct_message_connection(i)) {
          sleep(1000);
        }
      }
    }
    break;
  }
  default: {
    // Invalid message type. Do nothing and just ignore it.
    printf("Received invalid message from server\n");
    delete message;
    break;
  }
  }
}

bool Node::send_direct_message(size_t client_id, Message &msg) {
  bool ret_value =
      this->dm_manager->ensure_client_directly_connected(client_id);

  if (ret_value) {
    // Prepare the message ids
    msg.set_sender_id(this->id);
    msg.set_target_id(client_id);

    ret_value =
        this->dm_manager->send_direct_message(client_id, msg);
  }

  return ret_value;
}

size_t Node::get_node_id_with_wait() {
  if (this->id == -1) {
    // This means that the id has not been found yet. Wait for it to be
    // available
    this->enum_signal.wait();
  }

  return this->id;
}

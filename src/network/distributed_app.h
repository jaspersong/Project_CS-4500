/**
 * Name: Snowy Chen, Joe Song
 * Date: 21 February 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::CwC

#pragma once

#include "queue.h"

#include "network.h"
#include "recv_msg_manager.h"
#include "serial.h"

/**
 * A helper class that takes in serial data and waits until the data has
 * constructed a full message. This is because streamed data don't always
 * arrive at the same time, and as a result, messages can end up broken up in
 * the stream. This class puts that message together and then returns with a
 * fully constructed message.
 */
class MessageReceiver_ {
public:
  unsigned char *buffer;
  size_t buffer_index;

  MessageReceiver_() {
    this->buffer = new unsigned char[1024];
    this->buffer_index = 0;
  }

  ~MessageReceiver_() { delete[] this->buffer; }

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

    // TODO: Check for errors when building the message (i.e., continuous
    //  building of the message even though it's passed 1024 bytes, etc.)
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
 * An implementation of Server that multiple clients can register with. Once
 * the client has been connected to the server, it will then send clients the
 * addresses of all clients to each client (1 address per message).
 */
class Registrar : public Server {
public:
  // An array of client information. This array will have all the client
  // information in a contiguous fashion.
  Directory *client_dir_;

  MessageReceiver_ *message_receiver_;
  ReceivedMessageManager *tcp_msg_manager_;

  /**
   * Constructs a server with a fixed number of maximum number of clients.
   * @param ip_addr The IP address of the server. This string will be acquired.
   * @param port_num The port number of the server.
   * @param max_num_clients The fixed number of maximum number of clients
   *        that the server can register at any given time.
   * @param tcp_manager The TCP message manager that will handle receiving
   *        the messages
   */
  Registrar(String *ip_addr, int port_num, size_t max_num_clients,
            ReceivedMessageManager &tcp_manager)
      : Server(ip_addr, port_num, max_num_clients,
               Server::DEFAULT_MAX_RECEIVE_SIZE) {
    // Initialize the client directory
    this->client_dir_ = new Directory(max_num_clients);
    // Set the sender to the server id
    this->client_dir_->set_sender_id(-1);

    this->message_receiver_ = new MessageReceiver_();
    this->tcp_msg_manager_ = &tcp_manager;
  }

  /**
   * Deconstructs the server.
   */
  ~Registrar() override {
    // Close the server
    this->close_server();
    while (this->is_running()) {
    }

    // Free the appropriate memory
    delete this->client_dir_;
  }

  void init() override {
    // Nothing special is required to initialize.
  }

  void run_server() override {
    // Nothing special is required to run_server().
  }

  void handle_shutdown() override {
    // Nothing special is required.
  }

  void handle_incoming_message(size_t client_id, unsigned char *buffer,
                               size_t num_bytes) override {
    // Deserialize the data
    Message *message = this->message_receiver_->receive_data(buffer, num_bytes);
    if (message == nullptr) {
      // Received incomplete message. Wait for it.
      return;
    }
    switch (message->get_message_kind()) {
    case MsgKind::Ack: {
      this->tcp_msg_manager_->handle_ack(*message->as_ack());
      break;
    }
    case MsgKind::Nack: {
      this->tcp_msg_manager_->handle_nack(*message->as_nack());
      break;
    }
    case MsgKind::Put: {
      this->tcp_msg_manager_->handle_put(*message->as_put());
      break;
    }
    case MsgKind::Get: {
      this->tcp_msg_manager_->handle_get(*message->as_get());
      break;
    }
    case MsgKind::WaitAndGet: {
      this->tcp_msg_manager_->handle_waitandget(*message->as_waitandget());
      break;
    }
    case MsgKind::Status: {
      this->tcp_msg_manager_->handle_status(*message->as_status());
      break;
    }
    case MsgKind::Kill: {
      printf("Received Kill message from client id %zu\n", client_id);
      printf("Server cannot be killed. Ignoring command.\n");
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
      this->client_dir_->add_client(client_id, ip_addr,
                                    reg_message->get_port_num());

      // Now send out the updated directory to all of the other clients that
      // are connected.
      this->broadcast_client_directory_();

      delete ip_addr;
      break;
    }
    case MsgKind::Directory: {
      // Do nothing with a directory message because this is the registration
      // server. The server should be the only one sending these sort of
      // messages.
      printf("Received Directory message from client id %zu. Ignoring "
             "command\n",
             client_id);
      break;
    }
    default: {
      // Invalid message type. Do nothing and just ignore it.
      printf("Received invalid message from client id %zu. Ignoring.\n",
             client_id);
      break;
    }
    }

    delete message;
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
    this->client_dir_->remove_client(client_id);

    this->broadcast_client_directory_();
  }

  /**
   * Helper function that broadcasts the updated client directory to all of
   * the connected and registered nodes to this server.
   */
  void broadcast_client_directory_() {
    for (size_t i = 0; i < this->client_dir_->get_max_num_clients(); i++) {
      if (this->client_dir_->is_client_connected(i)) {
        this->client_dir_->set_target_id(i);

        // Serialize the directory
        Serializer serialized_broadcast;
        this->client_dir_->serialize(serialized_broadcast);
        unsigned char *broadcast = serialized_broadcast.get_serialized_buffer();
        size_t broadcast_size = serialized_broadcast.get_size_serialized_data();

        // Send the data
        this->send_message(i, broadcast, broadcast_size);
        printf("Sent directory message, size %zu to client id %zu\n",
               broadcast_size, i);
      }
    }
  }
};

/**
 * A helper class used to manage the direct connections to other clients
 * between Nodes.
 */
class ClientDMManager_ : public Server {
public:
  Directory *directory_;
  Lock *directory_lock_;

  MessageReceiver_ *message_receiver_;
  ReceivedMessageManager *tcp_msg_manager_;

  ClientDMManager_(String *ip_addr, int port_num, size_t max_clients,
                   ReceivedMessageManager &tcp_manager)
      : Server(ip_addr, port_num, max_clients, 1024) {
    this->directory_ = new Directory(max_clients);
    this->directory_lock_ = new Lock();

    this->message_receiver_ = new MessageReceiver_();
    this->tcp_msg_manager_ = &tcp_manager;
  }

  ~ClientDMManager_() override {
    this->close_server();
    delete this->directory_;
    delete this->directory_lock_;
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
    Message *message = this->message_receiver_->receive_data(buffer, num_bytes);
    if (message == nullptr) {
      return;
    }
    switch (message->get_message_kind()) {
    case MsgKind::Ack: {
      this->tcp_msg_manager_->handle_ack(*message->as_ack());
      break;
    }
    case MsgKind::Nack: {
      this->tcp_msg_manager_->handle_nack(*message->as_nack());
      break;
    }
    case MsgKind::Put: {
      this->tcp_msg_manager_->handle_put(*message->as_put());
      break;
    }
    case MsgKind::Get: {
      this->tcp_msg_manager_->handle_get(*message->as_get());
      break;
    }
    case MsgKind::WaitAndGet: {
      this->tcp_msg_manager_->handle_waitandget(*message->as_waitandget());
      break;
    }
    case MsgKind::Status: {
      this->tcp_msg_manager_->handle_status(*message->as_status());
      break;
    }
    case MsgKind::Kill: {
      printf("Received Kill message from client id %zu. Ignoring command.\n",
             client_id);
      break;
    }
    case MsgKind::Register: {
      printf("Received Register message from client id %zu. Ignoring "
             "command.\n",
             client_id);
      break;
    }
    case MsgKind::Directory: {
      printf("Received Directory message from client id %zu. Ignoring "
             "command.\n",
             client_id);
      break;
    }
    default: {
      // Invalid message type. Do nothing and just ignore it.
      printf("Received invalid message from client id %zu\n", client_id);
      break;
    }
    }

    delete message;
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
      if (this->directory_->is_client_connected(client_id)) {
        String *ip_addr = this->directory_->get_client_ip_addr(client_id);
        int port_num = this->directory_->get_client_port_num(client_id);
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
   * @param message A pointer to the buffer of the message that should be
   * sent. this message should be dynamically allocated, and once it is
   * passed into the function, this function will take ownership.
   * @param bytes The number of bytes of this message.
   * @return True if the message has successfully been queued. False if
   * otherwise.
   */
  bool send_direct_message(size_t client_id, unsigned char *message,
                           size_t bytes) {
    return this->send_message(client_id, message, bytes);
  }

  /**
   * Determines if the client is already directly connected to the specified
   * client.
   * @param client_id Unique client id that this client should be connecting
   * to.
   * @return True if the clients are directly connected. False if otherwise.
   */
  bool is_client_directly_connected(size_t client_id) {
    return this->is_client_connected(client_id);
  }

  /**
   * Updates the directory for the direct message manager.
   * @param new_directory The updated directory. The function will acquire
   * ownership of this data.
   */
  void update_directory(Directory *new_directory) {
    this->directory_lock_->lock();
    // Close sockets for clients that no longer are connected
    for (size_t i = 0; i < new_directory->get_max_num_clients(); i++) {
      if (!new_directory->is_client_connected(i)) {
        this->close_direct_message_connection(i);
      }
    }
    delete this->directory_;
    this->directory_ = new_directory;
    this->directory_lock_->unlock();
  }
};

/**
 * An implementation of Client that connects to a server and receives
 * addresses (1 address per incoming message) of other clients that have
 * connected to that server. Then the client can sustain direct communication
 * with the other clients.
 */
class Node : public Client {
public:
  ClientDMManager_ *dm_manager_;

  MessageReceiver_ *message_receiver_;
  ReceivedMessageManager *tcp_msg_manager_;

  /**
   * Constructs a client.
   * @param server_ip_addr The server IP address. The string is acquired.
   * @param server_port_num The port number of the server.
   * @param ip_addr The IP address of the client. The string is acquired
   * @param port_num The port number of this client.
   * @param fixed_num_clients The fixed number of clients that the client can
   *        directly communicate with after receiving the addresses from the
   *        server.
   * @param tcp_manager The TCP message manager that will handle receiving
   *        the messages from server and node.
   * @param dm_tcp_manager The TCP message manager that will handle receiving
   *        the messages going directly between nodes
   */
  Node(String *server_ip_addr, int server_port_num, String *ip_addr,
       int port_num, size_t fixed_num_clients,
       ReceivedMessageManager &tcp_manager,
       ReceivedMessageManager &dm_tcp_manager)
      : Client(server_ip_addr, server_port_num, fixed_num_clients) {
    this->dm_manager_ = new ClientDMManager_(ip_addr, port_num,
                                             fixed_num_clients, dm_tcp_manager);

    this->message_receiver_ = new MessageReceiver_();
    this->tcp_msg_manager_ = &tcp_manager;
  }

  /**
   * Deconstructs the client
   */
  ~Node() override {
    // Close the client
    this->close_client();
    this->dm_manager_->close_server();
    delete this->dm_manager_;
  }

  void init() override {
    // Start up the direct message manager
    this->dm_manager_->start();

    // Set up the register message
    Register reg(this->dm_manager_->ip_addr_, this->dm_manager_->port_num_);
    reg.set_sender_id(-1); // Don't know what the index is yet. Use the servers
    reg.set_target_id(-1); // Goes to the server

    // Serialize the message
    Serializer serialized_message;
    reg.serialize(serialized_message);
    unsigned char *msg = serialized_message.get_serialized_buffer();
    size_t num_bytes = serialized_message.get_size_serialized_data();

    this->send_message(msg, num_bytes);
  }

  void handle_shutdown() override { this->dm_manager_->close_server(); }

  void run_client() override {}

  void handle_incoming_message(unsigned char *buffer,
                               size_t num_bytes) override {
    // Deserialize the data
    Message *message = this->message_receiver_->receive_data(buffer, num_bytes);
    if (message == nullptr) {
      return;
    }
    switch (message->get_message_kind()) {
    case MsgKind::Ack: {
      this->tcp_msg_manager_->handle_ack(*message->as_ack());
      delete message;
      break;
    }
    case MsgKind::Nack: {
      this->tcp_msg_manager_->handle_nack(*message->as_nack());
      delete message;
      break;
    }
    case MsgKind::Put: {
      this->tcp_msg_manager_->handle_put(*message->as_put());
      delete message;
      break;
    }
    case MsgKind::Get: {
      this->tcp_msg_manager_->handle_get(*message->as_get());
      delete message;
      break;
    }
    case MsgKind::WaitAndGet: {
      this->tcp_msg_manager_->handle_waitandget(*message->as_waitandget());
      delete message;
      break;
    }
    case MsgKind::Status: {
      this->tcp_msg_manager_->handle_status(*message->as_status());
      delete message;
      break;
    }
    case MsgKind::Kill: {
      printf("Received a kill message from the server. Initiating client "
             "spin-down.\n");
      this->close_client_no_wait();
      delete message;
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
      this->dm_manager_->update_directory(message->as_directory());
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

  /**
   * Initiates a direct message connection to the specified client at the
   * client id.
   * @param client_id The unique client id, which will be reused if a client
   *        disconnects from the registrar server.
   * @return True if the direct connection was successful. False if otherwise.
   */
  bool initiate_direct_message_connection(size_t client_id) {
    return this->dm_manager_->initiate_direct_message_connection(client_id);
  }

  /**
   * Closes a direct message connection to the specified client at the client
   * id.
   * @param client_id The unique client id.
   */
  void close_direct_message_connection(size_t client_id) {
    this->dm_manager_->close_direct_message_connection(client_id);
  }

  /**
   * Sends a direct message to the client at the specified client id.
   * @param client_id The unique client id.
   * @param message A pointer to the buffer of the message that should be
   * sent. this message should be dynamically allocated, and once it is
   * passed into the function, this function will take ownership.
   * @param bytes The number of bytes of this message.
   * @return True if the message has successfully been queued. False if
   * otherwise.
   */
  bool send_direct_message(size_t client_id, unsigned char *message,
                           size_t bytes) {
    bool ret_value = this->dm_manager_->is_client_directly_connected(client_id);

    if (!ret_value) {
      // It hasn't been connected yet. Initiate a direct message connection
      ret_value =
          this->dm_manager_->initiate_direct_message_connection(client_id);
    }

    if (ret_value) {
      ret_value =
          this->dm_manager_->send_direct_message(client_id, message, bytes);
    }

    return ret_value;
  }
};

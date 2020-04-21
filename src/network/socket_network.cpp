/**
 * Name: Snowy Chen, Joe Song
 * Date: 21 March 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::Cpp

#include "socket_network.h"
#include "serial.h"
#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

/**
 * A private friend class of the network that manages individual connections.
 */
class SocketNetwork::Connection {
public:
  Connection(SocketNetwork *network, size_t connection_id) {
    assert(network);
    assert(connection_id < network->max_connections);

    this->network = network;
    this->connection_id = connection_id;
    this->socket = 0;

    this->msg_buffer_size = 1024;
    this->msg_buffer_index = 0;
    this->msg_buffer = new unsigned char[this->msg_buffer_size];
  }

  ~Connection() { delete[] this->msg_buffer; }

  size_t get_socket() {
    assert(this->is_connected());
    return this->socket;
  }

  bool is_connected() { return this->socket != 0; }

  // Assign this connection with an actively connected socket with another.
  void assign_connection(int connection_socket) {
    this->connection_lock.lock();
    assert(this->socket == 0);
    this->socket = connection_socket;
    this->connection_lock.unlock();
  }

  void close_connection() {
    // No need to do anything if the connection is already closed.
    this->connection_lock.lock();
    if (this->is_connected()) {
      close(this->socket);
      this->socket = 0;
      this->msg_buffer_index = 0;
    }
    this->connection_lock.unlock();
  }

  void send_message(Message &msg) {
    // Prepare the message to be sent
    msg.set_target_id(this->connection_id);
    msg.set_sender_id(this->network->id);
    Serializer serialized_message;
    msg.serialize(serialized_message);
    unsigned char *message = serialized_message.get_serialized_buffer();
    size_t message_size = serialized_message.get_size_serialized_data();

    this->connection_lock.lock();
    if (this->is_connected()) {
      send(this->socket, message, message_size, 0);
      sleep(50);
      this->network->println(StrBuff()
                                 .c("Sent ")
                                 .c(message_size)
                                 .c("byte message to ")
                                 .c(this->connection_id));
    }
    this->connection_lock.unlock();

    // Clean up
    delete[] message;
  }

  void read_socket() {
    // Prepare the buffer for the read
    if (this->msg_buffer_size - this->msg_buffer_index == 0) {
      // There isn't much room left. Increase the size
      this->msg_buffer_size *= this->msg_buffer_size;
      auto *new_buffer = new unsigned char[this->msg_buffer_size];
      for (size_t i = 0; i < this->msg_buffer_index; i++) {
        new_buffer[i] = this->msg_buffer[i];
      }
      delete[] this->msg_buffer;
      this->msg_buffer = new_buffer;
    }

    int bytes_read = -1;
    this->connection_lock.lock();
    if (this->is_connected()) {
      bytes_read = read(this->socket, this->msg_buffer + this->msg_buffer_index,
                        this->msg_buffer_size - this->msg_buffer_index);
    }
    this->connection_lock.unlock();

    if (bytes_read == 0) {
      // This is a closing connection.
      struct sockaddr_in addr {};
      const int addrlen = sizeof(addr);

      // Close the socket and then reset the connection information
      this->close_connection();

      // Call the callback
      this->network->println(StrBuff()
                                 .c("Connection id ")
                                 .c(connection_id)
                                 .c(" closed connection"));
      this->network->handle_closing_connection(this->connection_id);
    } else if (bytes_read != -1) {
      this->msg_buffer_index += bytes_read;

      // Read the buffer until we can't interpret anymore messages from this
      while (this->msg_buffer_index > 0) {
        // Interpret this as a message
        Message *full_msg = nullptr;
        Message *header = Message::deserialize_as_message_header(
            this->msg_buffer, this->msg_buffer_index);
        // Determine if it's a full message if the collected buffer is big
        // enough to hold the data that the message header promises
        if (header != nullptr) {
          if (header->get_payload_size() ==
              this->msg_buffer_index - Message::HEADER_SIZE) {
            delete header; // Don't need the header anymore

            // Deserialize the message and reset the buffer
            full_msg = Message::deserialize_as_message(this->msg_buffer,
                                                       this->msg_buffer_index);
            this->msg_buffer_index = 0;

            // Now handle the message
            this->network->handle_incoming_message(this->connection_id,
                                                   full_msg);
          } else if (header->get_payload_size() <
                     this->msg_buffer_index - Message::HEADER_SIZE) {
            delete header; // Don't need the header anymore

            full_msg = Message::deserialize_as_message(this->msg_buffer,
                                                       this->msg_buffer_index);

            // Move the left over to the beginning
            size_t prev_message_size =
                header->get_payload_size() + Message::HEADER_SIZE;
            for (size_t i = 0; i < this->msg_buffer_index - prev_message_size;
                 i++) {
              this->msg_buffer[i] = this->msg_buffer[i + prev_message_size];
            }
            this->msg_buffer_index = this->msg_buffer_index - prev_message_size;

            // Now handle the message
            this->network->handle_incoming_message(this->connection_id,
                                                   full_msg);
          } else {
            delete header; // Don't need the header anymore
            break;
          }
        }
      }
    }
  }

private:
  SocketNetwork *network;
  size_t connection_id;
  int socket;
  Lock connection_lock;

  unsigned char *msg_buffer;
  size_t msg_buffer_index;
  size_t msg_buffer_size;
};

SocketNetwork::SocketNetwork(size_t id, const char *ip_addr, int port_num,
                             size_t max_connections)
    : ip_addr(ip_addr) {
  assert(ip_addr);
  assert(max_connections > 0);

  this->continue_running = false;
  this->is_running = false;

  this->id = id;
  this->port_num = port_num;
  this->listening_socket = 0;

  // Initialize the incoming connections list
  this->max_connections = max_connections;
  this->connections.reserve(this->max_connections);
  for (size_t i = 0; i < this->max_connections; i++) {
    this->connections.push_back(new Connection(this, i));
  }
}

SocketNetwork::~SocketNetwork() {
  this->close_network();
  for (auto *connection : this->connections) {
    delete connection;
  }
}

void SocketNetwork::close_network() {
  if (this->is_running) {
    this->println(StrBuff().c("Shutting down network."));
    this->continue_running = false;
    this->join();
  }
}

bool SocketNetwork::is_id_connected(size_t connection_id) {
  assert(connection_id < this->max_connections);
  return (connection_id == this->id) ||
         (this->connections[connection_id]->is_connected());
}

void SocketNetwork::initiate_connection(size_t connection_id,
                                        String *connection_ip_addr,
                                        int connection_port_num) {
  assert(connection_id < this->max_connections);

  if (this->is_id_connected(connection_id)) {
    // No need to do anything if it's already connected
    return;
  }

  struct sockaddr_in connection {};
  connection.sin_family = AF_INET;
  connection.sin_port = htons(connection_port_num);

  // Initiate the connection
  int new_socket = socket(AF_INET, SOCK_STREAM, 0);
  assert(new_socket >= 0);
  assert(inet_pton(AF_INET, connection_ip_addr->c_str(), &connection.sin_addr) >
         0);
  assert(connect(new_socket, (struct sockaddr *)&connection,
                 sizeof(connection)) >= 0);

  // Add the new socket
  this->connections[connection_id]->assign_connection(new_socket);
  this->println(StrBuff().c("Initiated new connection to ").c(connection_id));

  // Give it a few seconds for the connection to fully establish
  Thread::sleep(2000);
}

void SocketNetwork::close_connection(size_t connection_id) {
  assert(connection_id < this->max_connections);

  if (!this->is_id_connected(connection_id)) {
    // No need to do anything if nothing's connected
    return;
  } else {
    this->connections[connection_id]->close_connection();
    this->handle_closing_connection(connection_id);
    this->println(StrBuff().c("Closed connection to ").c(connection_id));
  }
}

void SocketNetwork::send_message(size_t target_id, Message &msg) {
  assert(target_id < this->max_connections);
  this->connections[target_id]->send_message(msg);
}

void SocketNetwork::broadcast_message(Message &msg) {
  this->println(StrBuff().c("Broadcasting message."));
  for (size_t i = 0; i < this->max_connections; i++) {
    if (i != this->id) {
      this->connections[i]->send_message(msg);
    }
  }
}

void SocketNetwork::run() {
  // A lot of the code has been based off of the following tutorial:
  // https://www.geeksforgeeks.org/socket-programming-in-cc-handling-multiple
  // -clients-on-server-without-multi-threading/

  int activity;
  fd_set connection_selector{};
  int max_socket_desc = 0;

  this->continue_running = true;
  this->is_running = true;

  this->initialize_listening_socket();
  this->handle_initialization();

  while (this->continue_running) {
    // Yield and then lock for the loop
    Thread::yield();

    // Re-initialize the fd set in order to prepare for the polling.
    FD_ZERO(&connection_selector);

    // Add the master socket back into the set
    FD_SET(this->listening_socket, &connection_selector);
    max_socket_desc = this->listening_socket;

    // Add the child sockets to the set
    for (size_t i = 0; i < this->max_connections; i++) {
      if (this->connections[i]->is_connected()) {
        // This node is connected. Add it to the set.
        int connection = this->connections[i]->get_socket();
        FD_SET(connection, &connection_selector);

        if (connection > max_socket_desc) {
          // This connection's file descriptor is the largest file descriptor
          // thus far.
          max_socket_desc = connection;
        }
      }
    }

    // Poll for activity from one of the sockets.
    // TODO: Make the polling timeout be configurable
    struct timeval poll_receive_timeout {};
    poll_receive_timeout.tv_sec = 0;
    poll_receive_timeout.tv_usec = 500;
    activity = select(max_socket_desc + 1, &connection_selector, nullptr,
                      nullptr, &poll_receive_timeout);
    if (activity != 0) {
      // Something happened in one of the sockets. Handle it.
      if (FD_ISSET(this->listening_socket, &connection_selector)) {
        // Something happened on the listening socket, which means it's a new
        // connection from a new node.
        this->add_new_connection();
      }

      // Handle activity from the other connections
      for (size_t i = 0; i < this->max_connections; i++) {
        if (this->connections[i]->is_connected()) {
          int connection = this->connections[i]->get_socket();
          if (FD_ISSET(connection, &connection_selector)) {
            this->connections[i]->read_socket();
          }
        }
      }
    }
  }

  for (size_t i = 0; i < this->max_connections; i++) {
    this->connections[i]->close_connection();
  }

  close(this->listening_socket);
  this->listening_socket = 0;

  this->is_running = false;
  this->println(StrBuff().c("Shut down"));
}

void SocketNetwork::initialize_listening_socket() {
  int opt = 1;
  struct sockaddr_in addr {};

  // Create the server socket, and set it to allow multiple connections
  this->listening_socket = socket(AF_INET, SOCK_STREAM, 0);
  assert(this->listening_socket != 0);
  assert(setsockopt(this->listening_socket, SOL_SOCKET,
                    SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) != 0);

  // Type of socket created
  addr.sin_family = AF_INET;
  assert(inet_pton(AF_INET, this->ip_addr.c_str(), &addr.sin_addr) > 0);
  addr.sin_port = htons(this->port_num);

  // Attaching socket to the port specified
  assert(bind(this->listening_socket, (struct sockaddr *)&addr, sizeof(addr)) >=
         0);
  assert(listen(this->listening_socket, this->max_connections) >= 0);
}

void SocketNetwork::add_new_connection() {
  int new_socket;
  struct sockaddr_in addr {};
  const int addrlen = sizeof(addr);

  new_socket = accept(this->listening_socket, (struct sockaddr *)&addr,
                      (socklen_t *)&addrlen);
  assert(new_socket >= 0);

  // Find an empty connection id
  for (size_t i = 0; i < this->max_connections; i++) {
    if ((i != this->id) && (!this->connections[i]->is_connected())) {
      // Found an empty spot. Add it in
      this->connections[i]->assign_connection(new_socket);
      this->println(StrBuff().c("New id ").c(i).c(" initiated connection"));
      break;
    }
  }
}

void SocketNetwork::println(StrBuff &msg) {
#ifdef DEBUG_NETWORK_PRINT
  String *message = msg.get();
  printf("%zu: %s\n", this->id, message->c_str());
  delete message;
#else
  (void)msg;
  (void)this->id;
#endif
}

void SocketNetwork::self_shutdown() {
  this->println(StrBuff().c("Closing network down..."));
  this->continue_running = false;
}

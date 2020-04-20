/**
 * Name: Snowy Chen, Joe Song
 * Date: 21 March 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::Cpp

#include "network.h"
#include "serial.h"
#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

Network::Network(size_t id, const char *ip_addr, int port_num,
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
  this->connection_sockets = new int[this->max_connections];

  // Initialize the message receiver
  this->msg_buffer_size = 1024;
  this->msg_buffer_index = 0;
  this->msg_buffer = new unsigned char[this->msg_buffer_size];
}

Network::~Network() {
  this->close_network();

  delete[] this->connection_sockets;
  delete[] this->msg_buffer;
}

void Network::close_network() {
  if (this->is_running) {
    this->println(StrBuff().c("Shutting down network."));
    this->continue_running = false;
    this->join();
  }
}

bool Network::is_id_connected(size_t connection_id) {
  assert(connection_id < this->max_connections);
  return (connection_id == this->id) ||
         (this->connection_sockets[connection_id] != 0);
}

void Network::initiate_connection(size_t connection_id,
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
  this->connection_lock.lock();
  this->connection_sockets[connection_id] = new_socket;
  this->connection_lock.unlock();

  this->println(StrBuff().c("Initiated new connection to ").c(connection_id));

  // Give it a few seconds for the connection to fully establish
  Thread::sleep(2000);
}

void Network::close_connection(size_t connection_id) {
  assert(connection_id < this->max_connections);

  if (!this->is_id_connected(connection_id)) {
    // No need to do anything if nothing's connected
    return;
  } else {
    this->connection_lock.lock();
    close(this->connection_sockets[connection_id]);
    this->connection_sockets[connection_id] = 0;
    this->connection_lock.unlock();

    this->println(StrBuff().c("Closed connection to ").c(connection_id));
  }
}

void Network::send_message(size_t target_id, Message &msg) {
  assert(target_id < this->max_connections);

  // Prepare the message to be sent
  msg.set_target_id(target_id);
  msg.set_sender_id(this->id);
  Serializer serialized_message;
  msg.serialize(serialized_message);
  unsigned char *message = serialized_message.get_serialized_buffer();
  size_t message_size = serialized_message.get_size_serialized_data();

  // Send the message
  if (this->connection_sockets[target_id] != 0) {
    this->outgoing_msg_lock.lock();
    send(this->connection_sockets[target_id], message, message_size, 0);
    this->outgoing_msg_lock.unlock();
    sleep(50);
    this->println(StrBuff().c("Sent ").c(message_size).c("byte message to ").c(target_id));
  }

  // Clean up
  delete[] message;
}

void Network::broadcast_message(Message &msg) {
  this->println(StrBuff().c("Broadcasting message."));
  for (size_t i = 0; i < this->max_connections; i++) {
    if (i != this->id) {
      this->send_message(i, msg);
    }
  }
}

void Network::run() {
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
      int client = this->connection_sockets[i];
      if (client > 0) {
        // This client is connected. Add it to the set.
        FD_SET(client, &connection_selector);
      }
      if (client > max_socket_desc) {
        // This client's file descriptor is the largest file descriptor
        // thus far.
        max_socket_desc = client;
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
        // connection from a new client.
        this->add_new_connection();
      }

      // Handle activity from the other connections
      for (size_t i = 0; i < this->max_connections; i++) {
        int sd = this->connection_sockets[i];

        if (FD_ISSET(sd, &connection_selector)) {
          this->read_socket(i);
        }
      }
    }
  }

  this->connection_lock.lock();
  for (size_t i = 0; i < this->max_connections; i++) {
    if (this->connection_sockets[i] != 0) {
      close(this->connection_sockets[i]);
      this->connection_sockets[i] = 0;
    }
  }
  this->connection_lock.unlock();

  close(this->listening_socket);
  this->listening_socket = 0;

  this->is_running = false;
  this->println(StrBuff().c("Shut down"));
}

void Network::initialize_listening_socket() {
  int opt = 1;
  struct sockaddr_in addr {};

  // Initialize all the client sockets to 0, which means that they aren't
  // being used
  this->connection_lock.lock();
  for (size_t i = 0; i < this->max_connections; i++) {
    this->connection_sockets[i] = 0;
  }
  this->connection_lock.unlock();

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

void Network::add_new_connection() {
  int new_socket;
  struct sockaddr_in addr {};
  const int addrlen = sizeof(addr);

  new_socket = accept(this->listening_socket, (struct sockaddr *)&addr,
                      (socklen_t *)&addrlen);
  assert(new_socket >= 0);

  // Find an empty client position/id
  this->connection_lock.lock();
  for (size_t i = 0; i < this->max_connections; i++) {
    if ((i != this->id) && (this->connection_sockets[i] == 0)) {
      // Found an empty spot. Add it in, and call the handle callback function
      this->connection_sockets[i] = new_socket;
      this->println(StrBuff().c("New id ").c(i).c(" initiated connection"));
      break;
    }
  }
  this->connection_lock.unlock();
}

void Network::read_socket(int connection_id) {
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

  int bytes_read = read(this->connection_sockets[connection_id],
                        this->msg_buffer + this->msg_buffer_index,
                        this->msg_buffer_size - this->msg_buffer_index);

  if (bytes_read == 0) {
    // This is a closing connection.
    struct sockaddr_in addr {};
    const int addrlen = sizeof(addr);

    this->connection_lock.lock();
    getpeername(this->connection_sockets[connection_id],
                (struct sockaddr *)&addr, (socklen_t *)&addrlen);
    close(this->connection_sockets[connection_id]);
    this->connection_sockets[connection_id] = 0;
    this->connection_lock.unlock();

    // Call the callback
    this->println(StrBuff().c("Connection id ").c(
        connection_id).c(" closed connection"));
    this->handle_closing_connection(connection_id);
  } else if (bytes_read != -1) {
    this->msg_buffer_index += bytes_read;

    // Read the buffer until we can't interpret anymore messages from this
    while (this->msg_buffer_index > 0) {
      // Interpret this as a message
      Message *full_msg = nullptr;
      Message *header = Message::deserialize_as_message_header(
          this->msg_buffer, this->msg_buffer_index);
      // Determine if it's a full message if the collected buffer is big enough
      // to hold the data that the message header promises
      if (header != nullptr) {
        if (header->get_payload_size() ==
            this->msg_buffer_index - Message::HEADER_SIZE) {
          delete header; // Don't need the header anymore

          // Deserialize the message and reset the buffer
          full_msg = Message::deserialize_as_message(this->msg_buffer,
                                                     this->msg_buffer_index);
          this->msg_buffer_index = 0;

          // Now handle the message
          this->handle_incoming_message(connection_id, full_msg);
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
          this->handle_incoming_message(connection_id, full_msg);
        } else {
          delete header; // Don't need the header anymore
          break;
        }
      }
    }
  }
}

void Network::println(StrBuff &msg) {
#ifdef DEBUG_NETWORK_PRINT
  String *message = msg.get();
  printf("%zu: %s\n", this->id, message->c_str());
  delete message;
#else
  (void)msg;
  (void)this->id;
#endif
}

void Network::self_shutdown() {
  this->println(StrBuff().c("Closing network down..."));
  this->continue_running = false;
}

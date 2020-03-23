/**
 * Name: Snowy Chen, Joe Song
 * Date: 22 March 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::Cpp

#include <cstdlib>
#include <cstring>

#include <unistd.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "network.h"

/**
 * A private container that holds information of outgoing messages for the
 * Server and Client. This object should only be used by the abstract Server
 * and Client classes.
 *
 * The whole purpose is to allow message information to go into a queue of
 * outgoing messages in the client and the server.
 */
class OutgoingMessage_ : public CustomObject {
public:
  size_t client_id;
  unsigned char *sending_buffer;
  size_t message_size;

  /**
   * Constructs this container. The buffer is external.
   * @param client_id The destination client id. Only relevant to the Server.
   * @param sending_buffer Pointer to the buffer containing the outgoing
   *        message. The passed in buffer will now be owned by the container.
   * @param message_size Size of the message in bytes
   */
  OutgoingMessage_(size_t client_id, unsigned char *sending_buffer,
                   size_t message_size) {
    this->client_id = client_id;
    this->sending_buffer = sending_buffer;
    this->message_size = message_size;
  }

  /**
   * Deconstructs the container.
   */
  ~OutgoingMessage_() override {
    this->client_id = 0;
    delete[] this->sending_buffer;
    this->message_size = 0;
  }
};

Server::Server(String *ip_addr, int port_num)
    : Server(ip_addr, port_num, 1, Server::DEFAULT_MAX_RECEIVE_SIZE) {}

Server::Server(String *ip_addr, int port_num, size_t max_clients,
               size_t max_receive_size) {
  assert(ip_addr != nullptr);
  assert(max_clients > 0);
  assert(max_receive_size > 0);

  // Initialize the server information
  this->continue_running = true;
  this->is_running = false;

  this->ip_addr = ip_addr;
  this->port_num = port_num;
  this->server_fd = 0;

  // Initialize the client list
  this->max_clients = max_clients;
  this->clients_sockets = new int[this->max_clients];
  this->max_socket_descriptor = 0;
  this->loop_lock = new Lock();

  // Initialize the message buffers
  this->max_receive_size = max_receive_size;
  this->receive_buffer = new unsigned char[max_receive_size];
  this->outgoing_message_queue = new Queue();
  this->outgoing_queue_lock = new Lock();
}

Server::~Server() {
  this->close_server();

  // Wait until the server stops running
  while (this->running()) {
  }

  // Free the memory as appropriate
  delete this->ip_addr;
  delete[] this->clients_sockets;
  delete[] this->receive_buffer;
  while (this->outgoing_message_queue->getSize() != 0) {
    auto *message = dynamic_cast<OutgoingMessage_ *>(
        this->outgoing_message_queue->dequeue());
    delete message;
  }
  delete this->outgoing_message_queue;
  delete this->outgoing_queue_lock;
  delete this->loop_lock;
}

void Server::run() {
  // A lot of the code has been based off of the following tutorial:
  // https://www.geeksforgeeks.org/socket-programming-in-cc-handling-multiple
  // -clients-on-server-without-multi-threading/

  int activity;

  this->is_running = true;

  // Initialize the socket for the server
  this->initialize_server();

  this->init();

  while (this->continue_running) {
    // Yield and then lock for the loop
    Server::yield();
    this->loop_lock->lock();

    // Re-initialize the fd set in order to prepare for the polling.
    FD_ZERO(&this->socket_set);

    // Add the master socket back into the set
    FD_SET(this->server_fd, &this->socket_set);
    this->max_socket_descriptor = this->server_fd;

    // Add the child sockets to the set
    for (size_t i = 0; i < this->max_clients; i++) {
      int client = this->clients_sockets[i];
      if (client > 0) {
        // This client is connected. Add it to the set.
        FD_SET(client, &this->socket_set);
      }
      if (client > this->max_socket_descriptor) {
        // This client's file descriptor is the largest file descriptor
        // thus far.
        this->max_socket_descriptor = client;
      }
    }

    // Poll for activity from one of the sockets.
    // TODO: Make the polling timeout be configurable
    struct timeval poll_receive_timeout {};
    poll_receive_timeout.tv_sec = 0;
    poll_receive_timeout.tv_usec = 100;
    activity = select(this->max_socket_descriptor + 1, &this->socket_set,
                      nullptr, nullptr, &poll_receive_timeout);
    if (activity != 0) {
      // Something happened in one of the sockets. Handle it.
      if (FD_ISSET(this->server_fd, &this->socket_set)) {
        // Something happened on the master socket, which means it's a new
        // connection from a new client.
        this->add_new_connection();
      }

      // Handle the client connections now
      this->manage_client_connections();
    }

    if (this->outgoing_message_queue->getSize() > 0) {
      // A message is queued up. Send it.
      this->outgoing_queue_lock->lock();
      auto *msg_info = dynamic_cast<OutgoingMessage_ *>(
          this->outgoing_message_queue->dequeue());
      this->outgoing_queue_lock->unlock();

      // The queue should only ever contain valid outgoing message
      // information.
      assert(msg_info != nullptr);
      assert(msg_info->client_id < this->max_clients);

      // Send the message if the client is alive. Otherwise, just throw it
      // away.
      if (this->clients_sockets[msg_info->client_id] != 0) {
        send(this->clients_sockets[msg_info->client_id],
             msg_info->sending_buffer, msg_info->message_size, 0);
      }

      // No need for the sending buffer anymore. Clear it out now.
      delete msg_info;
    }

    this->run_server();
    this->loop_lock->unlock();
  }

  // Close the servers and the clients and then clean up
  this->handle_shutdown();
  for (size_t i = 0; i < this->max_clients; i++) {
    if (this->clients_sockets[i] != 0) {
      close(this->clients_sockets[i]);
      this->clients_sockets[i] = 0;
    }
  }
  close(this->server_fd);
  this->server_fd = 0;

  this->is_running = false;
}

void Server::close_server() {
  if (this->running()) {
    this->continue_running = false;
    this->join();
  }
}

bool Server::send_message(size_t client_id, unsigned char *message,
                          size_t bytes) {
  if (message == nullptr) {
    return false;
  }
  if ((client_id < 0) || (client_id > this->max_clients)) {
    return false;
  }

  // Queue this message up.
  auto *msg_info = new OutgoingMessage_(client_id, message, bytes);
  this->outgoing_queue_lock->lock();
  this->outgoing_message_queue->enqueue(msg_info);
  this->outgoing_queue_lock->unlock();
  return true;
}

bool Server::is_client_connected(size_t client_id) {
  if ((client_id < 0) || (client_id > this->max_clients)) {
    return false;
  } else {
    return (this->clients_sockets[client_id] != 0);
  }
}

bool Server::initiate_connection(size_t client_id, String *ip_addr,
                                 int port_num) {
  if ((client_id < 0) || (client_id > this->max_clients)) {
    return false;
  }

  // Make sure that there is no connection already
  if (this->is_client_connected(client_id)) {
    return false;
  }

  struct sockaddr_in connection {};
  connection.sin_family = AF_INET;
  connection.sin_port = htons(port_num);

  // Initiate the connection
  int new_socket = socket(AF_INET, SOCK_STREAM, 0);
  assert(new_socket >= 0);
  assert(inet_pton(AF_INET, ip_addr->c_str(), &connection.sin_addr) > 0);
  assert(connect(new_socket, (struct sockaddr *)&connection,
                 sizeof(connection)) >= 0);

  // Add the new socket
  this->loop_lock->lock();
  this->clients_sockets[client_id] = new_socket;

  // Call the handle incoming connection, even though the server initiated
  // the server.
  this->handle_incoming_connection(client_id, ip_addr, port_num);

  this->loop_lock->unlock();
  return true;
}

void Server::close_connection(size_t client_id) {
  if ((client_id < 0) || (client_id > this->max_clients)) {
    return;
  }

  if (this->is_client_connected(client_id)) {
    this->loop_lock->lock();

    close(this->clients_sockets[client_id]);
    this->clients_sockets[client_id] = 0;

    // Call the handle closing connection since we just closed a connection.
    this->handle_closing_connection(client_id);

    this->loop_lock->unlock();
  }
}

void Server::initialize_server() {
  int opt = 1;
  struct sockaddr_in addr {};
  const int addrlen = sizeof(addr);

  // Initialize all the client sockets to 0, which means that they aren't
  // being used
  for (size_t i = 0; i < this->max_clients; i++) {
    this->clients_sockets[i] = 0;
  }

  // Create the server socket, and set it to allow multiple connections
  this->server_fd = socket(AF_INET, SOCK_STREAM, 0);
  assert(this->server_fd != 0);
  assert(setsockopt(this->server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                    &opt, sizeof(opt)) != 0);

  // Type of socket created
  addr.sin_family = AF_INET;
  assert(inet_pton(AF_INET, this->ip_addr->c_str(), &addr.sin_addr) > 0);
  addr.sin_port = htons(this->port_num);

  // Attaching socket to the port specified
  assert(bind(this->server_fd, (struct sockaddr *)&addr, sizeof(addr)) >= 0);
  assert(listen(this->server_fd, 3) >= 0);
}

void Server::add_new_connection() {
  int new_socket;
  struct sockaddr_in addr {};
  const int addrlen = sizeof(addr);

  new_socket =
      accept(this->server_fd, (struct sockaddr *)&addr, (socklen_t *)&addrlen);
  assert(new_socket >= 0);

  // Find an empty client position/id
  for (size_t i = 0; i < this->max_clients; i++) {
    if (this->clients_sockets[i] == 0) {
      // Found an empty spot. Add it in, and call the handle callback function
      this->clients_sockets[i] = new_socket;

      // Get the address of the client
      struct in_addr ip_addr = addr.sin_addr;
      char addr_cstr[INET_ADDRSTRLEN];
      inet_ntop(AF_INET, &ip_addr, addr_cstr, INET_ADDRSTRLEN);
      auto *addr_str = new String(addr_cstr);

      this->handle_incoming_connection(i, addr_str, (int)ntohs(addr.sin_port));

      delete addr_str;

      break;
    }
  }
}

void Server::manage_client_connections() {
  struct sockaddr_in addr {};
  const int addrlen = sizeof(addr);

  for (size_t i = 0; i < this->max_clients; i++) {
    int sd = this->clients_sockets[i];

    if (FD_ISSET(sd, &this->socket_set)) {
      // The activity is on this client. Read from it
      int bytes_read = read(sd, this->receive_buffer, this->max_receive_size);

      if (bytes_read == 0) {
        // This is a closing connection.
        getpeername(sd, (struct sockaddr *)&addr, (socklen_t *)&addrlen);
        close(sd);
        this->clients_sockets[i] = 0;

        // Call the callback
        this->handle_closing_connection(i);
      } else if (bytes_read != -1) {
        // This is a received message. Call the callback function
        this->handle_incoming_message(i, this->receive_buffer, bytes_read);
      }
    }
  }
}

Client::Client(String *server_ip_addr, int server_port_num)
    : Client(server_ip_addr, server_port_num,
             Client::DEFAULT_MAX_RECEIVE_SIZE) {}

Client::Client(String *server_ip_addr, int server_port_num,
               size_t max_receive_size) {
  this->continue_running = true;
  this->is_running = false;

  this->server_ip_addr = server_ip_addr;
  this->server_port_num = server_port_num;
  this->server_fd = 0;

  // Initialize the message buffers
  this->max_receive_size = max_receive_size;
  this->receive_buffer = new unsigned char[max_receive_size];
  this->outgoing_message_queue = new Queue();
  this->outgoing_queue_lock = new Lock();
}

Client::~Client() {
  this->close_client();

  // Wait until the server stops running
  while (this->running()) {
  }

  // Free the memory as appropriate
  delete this->server_ip_addr;
  delete[] this->receive_buffer;
  while (this->outgoing_message_queue->getSize() != 0) {
    auto *message = dynamic_cast<OutgoingMessage_ *>(
        this->outgoing_message_queue->dequeue());
    delete message;
  }
  delete this->outgoing_message_queue;
  delete this->outgoing_queue_lock;
}

void Client::run() {
  // Initialize the socket for the client
  struct sockaddr_in serv {};

  this->is_running = true;

  this->server_fd = socket(AF_INET, SOCK_STREAM, 0);
  assert(this->server_fd >= 0);
  serv.sin_family = AF_INET;
  serv.sin_port = htons(this->server_port_num);

  // Convert IP address from text to binary form
  assert(inet_pton(AF_INET, this->server_ip_addr->c_str(), &serv.sin_addr) >
         0);
  assert(connect(this->server_fd, (struct sockaddr *)&serv, sizeof(serv)) >=
         0);

  // Give some time for the server to finish establishing the connection
  // with this new client.
  sleep(5000); // 5 seconds

  this->init();

  while (this->continue_running) {
    // Re-initialize the file descriptor set to prepare the polling
    FD_ZERO(&this->fd_selector);
    FD_SET(this->server_fd, &this->fd_selector);

    // Poll for activity from the socket
    // TODO: Make the polling timeout be configurable
    struct timeval poll_receive_timeout {};
    poll_receive_timeout.tv_sec = 0;
    poll_receive_timeout.tv_usec = 100;
    int activity = select(this->server_fd + 1, &this->fd_selector, NULL, NULL,
                          &poll_receive_timeout);
    if (activity != 0) {
      // The activity is on this client. Read from it. It's okay to read
      // directly because it is the only one in the file descriptor selector.
      int bytes_read = read(this->server_fd, this->receive_buffer,
                            this->max_receive_size);
      if (bytes_read == 0) {
        // This is a closing connection.
        this->handle_closing_connection();
        this->continue_running = false;
        break;
      } else if (bytes_read != -1) {
        // this is a received message. Call the callback function
        this->handle_incoming_message(this->receive_buffer, bytes_read);
      }
    }

    if (this->outgoing_message_queue->getSize() > 0) {
      // A message is queued up. Send it.
      this->outgoing_queue_lock->lock();
      auto *msg_info = dynamic_cast<OutgoingMessage_ *>(
          this->outgoing_message_queue->dequeue());
      this->outgoing_queue_lock->unlock();

      // The queue should only ever contain valid outgoing message
      // information.
      assert(msg_info != nullptr);

      // Send the message
      send(this->server_fd, msg_info->sending_buffer, msg_info->message_size,
           0);

      // No need for the sending buffer anymore. Clear it out now.
      delete msg_info;
    }

    this->run_client();
  }

  // Close the client
  this->handle_shutdown();
  close(this->server_fd);
  this->server_fd = 0;

  this->is_running = false;
}

void Client::close_client() {
  if (this->running()) {
    this->continue_running = false;
    this->join();
  }
}

bool Client::send_message(unsigned char *message, size_t bytes) {
  if (message == nullptr) {
    return false;
  }

  // Queue this message up.
  auto *msg_info = new OutgoingMessage_(0, message, bytes);
  this->outgoing_queue_lock->lock();
  this->outgoing_message_queue->enqueue(msg_info);
  this->outgoing_queue_lock->unlock();
  return true;
}

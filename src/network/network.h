/**
 * Name: Snowy Chen, Joe Song
 * Date: 21 February 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::CwC

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "custom_object.h"
#include "custom_string.h"
#include "queue.h"
#include "thread.h"

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
  ~OutgoingMessage_() {
    this->client_id = 0;
    delete[] this->sending_buffer;
    this->message_size = 0;
  }
};

/**
 * An abstract thread class for a server using the POSIX networking library
 * functions. It will handle all of the server functionality, and will call
 * abstracted callback functions depending on the scenario (i.e., receiving
 * message, incoming connection, closing connection, run_server, etc.). Classes
 * inheriting from Server should implement the virtual functions in order to
 * properly handle the server actions.
 */
class Server : public Thread {
public:
  static const size_t DEFAULT_MAX_RECEIVE_SIZE = 1024;

  bool continue_running_;
  bool is_running_;

  String *ip_addr_;
  int port_num_;
  int server_fd_;       // File descriptor of the server
  int *client_sockets_; // Array of file descriptors for all the clients. 0
                        // if it is not used
  fd_set socket_set_;   // A socket set selector to manage multiple clients
  int max_socket_descriptor_; // The current maximum file descriptor.
  Lock *loop_lock_; // Locks whenever a server loop is running

  unsigned char *receive_buffer_;
  Queue *outgoing_message_queue_;
  Lock *outgoing_queue_lock_;

  // Configuration fields
  size_t max_clients_;
  size_t max_receive_size_;

  /**
   * Constructs a server. All inheriting classes should be calling one of the
   * constructors upon their construction as well, in order for the concrete
   * functions in this abstract class to function as expected.
   *
   * NOTE: This instance supports a maximum of 1 client.
   * @param ip_addr The IP address of the server. This string will be acquired.
   * @param port_num The port number of the server.
   */
  Server(String *ip_addr, int port_num)
      : Server(ip_addr, port_num, 1, Server::DEFAULT_MAX_RECEIVE_SIZE) {}

  /**
   * Constructs a server. All inheriting classes should be calling one of the
   * constructors upon their constructions as well in order fo the concrete
   * functions in this abstract class to function as expected
   * @param ip_addr The IP address of the server. This string will be acquired.
   * @param port_num The port number of the server.
   * @param max_clients The maximum number of clients this server should handle.
   * @param max_receive_size The maximum number of bytes this server can
   *                         receive.
   */
  Server(String *ip_addr, int port_num, size_t max_clients,
         size_t max_receive_size) {
    assert(ip_addr != nullptr);
    assert(max_clients > 0);
    assert(max_receive_size > 0);

    // Initialize the server information
    this->continue_running_ = true;
    this->is_running_ = false;

    this->ip_addr_ = ip_addr;
    this->port_num_ = port_num;
    this->server_fd_ = 0;

    // Initialize the client list
    this->max_clients_ = max_clients;
    this->client_sockets_ = new int[this->max_clients_];
    this->max_socket_descriptor_ = 0;
    this->loop_lock_ = new Lock();

    // Initialize the message buffers
    this->max_receive_size_ = max_receive_size;
    this->receive_buffer_ = new unsigned char[max_receive_size];
    this->outgoing_message_queue_ = new Queue();
    this->outgoing_queue_lock_ = new Lock();
  }

  /**
   * Deconstructs a server. The server should NOT be deconstructed shortly
   * after start() gets called--it should be allowed at least 5 seconds to
   * run. The deconstructor also closes the server if it is running.
   */
  ~Server() {
    this->close_server();

    // Wait until the server stops running
    while (this->is_running()) {
    }

    // Free the memory as appropriate
    delete this->ip_addr_;
    delete[] this->client_sockets_;
    delete[] this->receive_buffer_;
    while (this->outgoing_message_queue_->getSize() != 0) {
      OutgoingMessage_ *message = dynamic_cast<OutgoingMessage_ *>(
          this->outgoing_message_queue_->dequeue());
      delete message;
    }
    delete this->outgoing_message_queue_;
    delete this->outgoing_queue_lock_;
    delete this->loop_lock_;
  }

  /**
   * An abstract function that a server implementation should implement. This
   * function will be called right before the infinite loop in start_server(),
   * but after all other generic server properties have completed
   * initialization.
   *
   * NOTE: This function should only be implemented, but never called by any
   * code external to the original Server class.
   */
  virtual void init() { return; }

  /**
   * An abstract function that a server implementation should implement. This
   * function will be run_server upon every loop iteration of the server until
   * the close_client() function gets called.
   *
   * The time interval between the function call is dependent on how long the
   * server polls for activity in any of its connections.
   *
   * NOTE: This function should only be implemented, but never called by any
   * code external to the original Server class.
   */
  virtual void run_server() { return; }

  /**
   * An abstract function that a server implementation should implement. This
   * function will handle any message that is received by the server from a
   * client at client_id.
   *
   * NOTE: This function should only be implemented, but never called by any
   * code external to the original Server class.
   * @param client_id The client id. This value will be from 0 to the maximum
   *        number of clients the server can handle (exclusive).
   * @param buffer The buffer containing the message received by the client.
   *        This value is gaurunteed to never be null, unless this function
   *        has been called outside of the Server class. This buffer should
   *        never be freed by the server implementation, and shall remain
   *        external to the server implementation.
   * @param num_bytes The number of bytes of the message.
   */
  virtual void handle_incoming_message(size_t client_id,
                                       unsigned char *buffer,
                                       size_t num_bytes) {
    return;
  }

  /**
   * An abstract function that a server implementation should implement. this
   * function will handle any new, incoming connection by a client at client_id.
   *
   * NOTE: This function should only be implemented, but never called by any
   * code external to the original Server class.
   * @param new_client_id The client_id of the new connection. This value
   *        will be from 0 to the maximum number of clients the server can
   *        handle (exclusive). This id will be reused if and only if a
   *        previous connection at this client id had closed.
   * @param addr The address of the new connection. This string will remain
   *        external to the Server implementation.
   * @param port_num The port number of the new connection.
   */
  virtual void handle_incoming_connection(size_t new_client_id, String *addr,
                                          int port_num) {
    return;
  }

  /**
   * An abstract function that the server implementation should implement.
   * This function will handle any closing connection by a client at the
   * client id.
   *
   * NOTE: This function should only be implemented, but never called by any
   * code external to the original Server class.
   * @param client_id The client id of the closing connection. This value
   *        will be from 0 to the maximum number of clients the server can
   *        handle (exclusive). Once this function returns, the provided id can
   *        be reused by new incoming client connections.
   */
  virtual void handle_closing_connection(size_t client_id) { return; }

  /**
   * Runs the server in an infinite loop. The server should be started by
   * calling the thread's start() function.
   * @throws If the server cannot start up properly, it will terminate the
   *         program.
   */
  void run() override {
    // A lot of the code has been based off of the following tutorial:
    // https://www.geeksforgeeks.org/socket-programming-in-cc-handling-multiple
    // -clients-on-server-without-multi-threading/

    int activity;

    this->is_running_ = true;

    // Initialize the socket for the server
    this->initialize_server_();

    this->init();

    while (this->continue_running_) {
      // Yield and then lock for the loop
      this->yield();
      this->loop_lock_->lock();

      // Re-initialize the fd set in order to prepare for the polling.
      FD_ZERO(&this->socket_set_);

      // Add the master socket back into the set
      FD_SET(this->server_fd_, &this->socket_set_);
      this->max_socket_descriptor_ = this->server_fd_;

      // Add the child sockets to the set
      for (size_t i = 0; i < this->max_clients_; i++) {
        int client = this->client_sockets_[i];
        if (client > 0) {
          // This client is connected. Add it to the set.
          FD_SET(client, &this->socket_set_);
        }
        if (client > this->max_socket_descriptor_) {
          // This client's file descriptor is the largest file descriptor
          // thus far.
          this->max_socket_descriptor_ = client;
        }
      }

      // Poll for activity from one of the sockets.
      // TODO: Make the polling timeout be configurable
      struct timeval poll_receive_timeout;
      poll_receive_timeout.tv_sec = 0;
      poll_receive_timeout.tv_usec = 100;
      activity = select(this->max_socket_descriptor_ + 1, &this->socket_set_,
                        NULL, NULL, &poll_receive_timeout);
      if (activity != 0) {
        // Something happened in one of the sockets. Handle it.
        if (FD_ISSET(this->server_fd_, &this->socket_set_)) {
          // Something happened on the master socket, which means it's a new
          // connection from a new client.
          this->add_new_connection_();
        }

        // Handle the client connections now
        this->manage_client_connections_();
      }

      if (this->outgoing_message_queue_->getSize() > 0) {
        // A message is queued up. Send it.
        this->outgoing_queue_lock_->lock();
        OutgoingMessage_ *msg_info = dynamic_cast<OutgoingMessage_ *>(
            this->outgoing_message_queue_->dequeue());
        this->outgoing_queue_lock_->unlock();

        // The queue should only ever contain valid outgoing message
        // information.
        assert(msg_info != nullptr);
        assert(msg_info->client_id < this->max_clients_);

        // Send the message if the client is alive. Otherwise, just throw it
        // away.
        if (this->client_sockets_[msg_info->client_id] != 0) {
          send(this->client_sockets_[msg_info->client_id],
               msg_info->sending_buffer, msg_info->message_size, 0);
        }

        // No need for the sending buffer anymore. Clear it out now.
        delete msg_info;
      }

      this->run_server();
      this->loop_lock_->unlock();
    }

    // Close the servers and the clients and then clean up
    for (size_t i = 0; i < this->max_clients_; i++) {
      if (this->client_sockets_[i] != 0) {
        close(this->client_sockets_[i]);
        this->client_sockets_[i] = 0;
      }
    }
    close(this->server_fd_);
    this->server_fd_ = 0;

    this->is_running_ = false;
  }

  /**
   * Closes the server. This function can be called by other processes, or it
   * can be called within the same class itself.
   *
   * NOTE: This function will not immediately close the server. It will
   * finish one iteration of its run loop before it fully closes.
   */
  void close_server() {
    if (this->is_running()) {
      this->continue_running_ = false;
      this->join();
    }
  }

  /**
   * Determines if the server is running.
   * @return True if the server is running, false if otherwise.
   */
  bool is_running() { return this->is_running_; }

  /**
   * Sends a message to a client that is connected to the server.
   *
   * If the message buffer provided is null, the server will treat it as if
   * there is no message to send.
   * @param client_id The id of the client the message should be sent to.
   * @param message An array of bytes containing the message with the
   *                specified size. This message should be dynamically
   *                allocated, and once it is passed into the function, the
   *                server will delete/free that memory once it has
   *                successfully sent the message.
   * @param bytes How many bytes the message is.
   * @return True if the message has successfully been queued to be sent.
   *         False if not (i.e., the client_id is invalid, the message is
   *         null, etc.)
   */
  bool send_message(size_t client_id, unsigned char *message, size_t bytes) {
    if (message == nullptr) {
      return false;
    }
    if ((client_id < 0) || (client_id > this->max_clients_)) {
      return false;
    }

    // Queue this message up.
    OutgoingMessage_ *msg_info =
        new OutgoingMessage_(client_id, message, bytes);
    this->outgoing_queue_lock_->lock();
    this->outgoing_message_queue_->enqueue(msg_info);
    this->outgoing_queue_lock_->unlock();
    return true;
  }

  /**
   * Determines if the provided client id is connected.
   * @param client_id The client id to query
   * @return True if there is a client connected at this id. False if
   * otherwise (including when the client id is out of bounds).
   */
  bool is_client_connected(size_t client_id) {
    if ((client_id < 0) || (client_id > this->max_clients_)) {
      return false;
    }
    else {
      return (this->client_sockets_[client_id] != 0);
    }
  }

  /**
   * Initiates a connection to a specified IP address and port number. It
   * will attach this connection to the specified client id.
   * @param client_id The unique client id.
   * @param ip_addr The IP address to connect to.
   * @param port_num The port number that we want to connect to.
   * @return True if the connection was successful. False if otherwise (i.e.,
   * the client id has already been connected to another client).
   */
  bool initiate_connection(size_t client_id, String *ip_addr, int port_num) {
    if ((client_id < 0) || (client_id > this->max_clients_)) {
      return false;
    }

    // Make sure that there is no connection already
    if (this->is_client_connected(client_id)) {
      return false;
    }

    struct sockaddr_in connection;
    connection.sin_family = AF_INET;
    connection.sin_port = htons(port_num);

    // Initiate the connection
    int new_socket = 0;
    assert((new_socket = socket(AF_INET, SOCK_STREAM, 0)) >= 0);
    assert(inet_pton(AF_INET, ip_addr->c_str(), &connection.sin_addr) >
           0);
    assert(connect(new_socket, (struct sockaddr *)&connection,
        sizeof(connection)) >= 0);

    // Add the new socket
    this->loop_lock_->lock();
    this->client_sockets_[client_id] = new_socket;

    // Call the handle incoming connection, even though the server initiated
    // the server.
    this->handle_incoming_connection(client_id, ip_addr, port_num);

    this->loop_lock_->unlock();
    return true;
  }

  /**
   * Forcibly close a connection with a client at the specified id.
   * @param client_id The unique id of the client to close connection with.
   */
  void close_connection(size_t client_id) {
    if ((client_id < 0) || (client_id > this->max_clients_)) {
      return;
    }

    if (this->is_client_connected(client_id)) {
      this->loop_lock_->lock();

      close(this->client_sockets_[client_id]);
      this->client_sockets_[client_id] = 0;

      // Call the handle closing connection since we just closed a connection.
      this->handle_closing_connection(client_id);

      this->loop_lock_->unlock();
    }
  }

  /****************************
   * Configuration functions
   ****************************/

  /**
   * Gets the maximum number of clients this server is configured to support.
   * By default, it supports only 1 client.
   * @return Maximum number of clients.
   */
  size_t get_max_clients() { return this->max_clients_; }

  /**
   * Gets the maximum number of bytes the server can receive from a client.
   * @return The maximum number of bytes a received message can be.
   */
  size_t get_max_receive_size() { return this->max_receive_size_; }

  /****************************
   * Private internal functions that should not be called externally.
   ****************************/

  /**
   * Private function to initialize the server. This function should never be
   * called externally.
   */
  void initialize_server_() {
    int opt = 1;
    struct sockaddr_in addr;
    const int addrlen = sizeof(addr);

    // Initialize all the client sockets to 0, which means that they aren't
    // being used
    for (size_t i = 0; i < this->max_clients_; i++) {
      this->client_sockets_[i] = 0;
    }

    // Create the server socket, and set it to allow multiple connections
    assert((this->server_fd_ = socket(AF_INET, SOCK_STREAM, 0)) != 0);
    assert(setsockopt(this->server_fd_, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                      &opt, sizeof(opt)) != 0);

    // Type of socket created
    addr.sin_family = AF_INET;
    assert(inet_pton(AF_INET, this->ip_addr_->c_str(), &addr.sin_addr) > 0);
    addr.sin_port = htons(this->port_num_);

    // Attaching socket to the port specified
    assert(bind(this->server_fd_, (struct sockaddr *)&addr, sizeof(addr)) >= 0);
    assert(listen(this->server_fd_, 3) >= 0);
  }

  /**
   * Private function handling new incoming connection.
   */
  void add_new_connection_() {
    int new_socket;
    struct sockaddr_in addr;
    const int addrlen = sizeof(addr);

    assert((new_socket = accept(this->server_fd_, (struct sockaddr *)&addr,
                                (socklen_t *)&addrlen)) >= 0);
    // Find an empty client position/id
    for (size_t i = 0; i < this->max_clients_; i++) {
      if (this->client_sockets_[i] == 0) {
        // Found an empty spot. Add it in, and call the handle callback function
        this->client_sockets_[i] = new_socket;

        // Get the address of the client
        struct in_addr ip_addr = addr.sin_addr;
        char addr_cstr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &ip_addr, addr_cstr, INET_ADDRSTRLEN);
        String *addr_str = new String(addr_cstr);

        this->handle_incoming_connection(i, addr_str,
                                         (int)ntohs(addr.sin_port));

        delete addr_str;

        break;
      }
    }
  }

  /**
   * Private function that manages activity on the client connections.
   */
  void manage_client_connections_() {
    struct sockaddr_in addr;
    const int addrlen = sizeof(addr);

    for (size_t i = 0; i < this->max_clients_; i++) {
      int sd = this->client_sockets_[i];

      if (FD_ISSET(sd, &this->socket_set_)) {
        // The activity is on this client. Read from it
        int bytes_read =
            read(sd, this->receive_buffer_, this->max_receive_size_);

        if (bytes_read == 0) {
          // This is a closing connection.
          getpeername(sd, (struct sockaddr *)&addr, (socklen_t *)&addrlen);
          close(sd);
          this->client_sockets_[i] = 0;

          // Call the callback
          this->handle_closing_connection(i);
        } else if (bytes_read != -1) {
          // This is a received message. Call the callback function
          this->handle_incoming_message(i, this->receive_buffer_, bytes_read);
        }
      }
    }
  }
};

/**
 * An abstract thread class for a client using the POSIX networking library
 * functions. It will handle all of the client functionality, and will call
 * abstracted callback functions depending on the scenario. Classes
 * inheriting from Server should implement the virtual functions in order to
 * properly handle the client actions.
 */
class Client : public Thread {
public:
  static const size_t DEFAULT_MAX_RECEIVE_SIZE = 1024;

  bool continue_running_;
  bool is_running_;

  String *server_ip_addr_;
  int server_port_num_;
  int server_fd_;
  fd_set fd_selector_;

  unsigned char *receive_buffer_;
  Queue *outgoing_message_queue_;
  Lock *outgoing_queue_lock_;

  // Configuration fields
  size_t max_receive_size_;

  /**
   * Constructs a client. All inheriting classes should be calling this
   * constructor upon their construction as well, in order for the concrete
   * functions in this abstract class to function as expected.
   * @param server_ip_addr The server IP address. The string is acquired.
   * @param server_port_num The port number of the server.
   */
  Client(String *server_ip_addr, int server_port_num)
      : Client(server_ip_addr, server_port_num,
               Client::DEFAULT_MAX_RECEIVE_SIZE) {}

  /**
   * Constructs a client. All inheriting classes should be calling one of the
   * constructors upon their constructions as well in order for the concrete
   * functions in this abstract class to function as expected.
   * @param server_ip_addr The server IP address. The string is acquired.
   * @param server_port_num The port number of the server.
   * @param max_receive_size The maximum number of bytes this server can
   *                         receive.
   */
  Client(String *server_ip_addr, int server_port_num, size_t max_receive_size) {
    this->continue_running_ = true;
    this->is_running_ = false;

    this->server_ip_addr_ = server_ip_addr;
    this->server_port_num_ = server_port_num;
    this->server_fd_ = 0;

    // Initialize the message buffers
    this->max_receive_size_ = max_receive_size;
    this->receive_buffer_ = new unsigned char[max_receive_size];
    this->outgoing_message_queue_ = new Queue();
    this->outgoing_queue_lock_ = new Lock();
  }

  /**
   * Deconstructs a client. The client should NOT be deconstructed shortly
   * after start() gets called--it should be allowed at least 10 seconds to
   * run. The deconstructor also closes the client if it is running.
   */
  virtual ~Client() {
    this->close_client();

    // Wait until the server stops running
    while (this->is_running()) {
    }

    // Free the memory as appropriate
    delete this->server_ip_addr_;
    delete[] this->receive_buffer_;
    while (this->outgoing_message_queue_->getSize() != 0) {
      OutgoingMessage_ *message = dynamic_cast<OutgoingMessage_ *>(
          this->outgoing_message_queue_->dequeue());
      delete message;
    }
    delete this->outgoing_message_queue_;
    delete this->outgoing_queue_lock_;
  }

  /**
   * An abstract function that a client implementation should implement. This
   * function will be called right before the infinite loop in start_client(),
   * but after all other generic client properties (i.e., connecting to the
   * server) have completed initialization.
   *
   * NOTE: This function should only be implemented, but never called by any
   * code external to the original Client class.
   */
  virtual void init() {}

  /**
   * An abstract function that a client implementation should implement. This
   * function will be run upon every loop iteration of the client until the
   * close_client() function gets called.
   *
   * The time interval between the function call is dependent on how long the
   * client polls for activity in any of its connections.
   *
   * NOTE: This function should only be implemented, but never called by any
   * code external to the original Client class.
   */
  virtual void run_client() {}

  /**
   * An abstract function that a client implementation should implement. This
   * function will handle any message that is received by the client from the
   * server.
   *
   * NOTE: This function should only be implemented, but never called by any
   * code external to the original Client class.
   * @param buffer The buffer containing the message received by the client.
   *        This value is gaurunteed to never be null, unless this function
   *        has been called outside of the Server class. This buffer should
   *        never be freed by the Client implementation, and shall remain
   *        external to the Client implementation.
   * @param num_bytes The number of bytes of the message.
   */
  virtual void handle_incoming_message(unsigned char *buffer,
                                       size_t num_bytes) {}

  /**
   * An abstract function that the client implementation should implement.
   * This function will handle a closing connection between the client and
   * the server.
   *
   * NOTE: This function should only be implemented, but never called by any
   * code external to the original Client class.
   */
  virtual void handle_closing_connection() {}

  /**
   * Starts the client. It will run on an infinite loop, calling the
   * abstracted function run(), which should be implemented by children
   * classes, upon each loop iteration, until the function close_client() gets
   * called.
   * @throws If the client cannot start up properly, and it cannot connect
   *         the spcified server at the provided IP address, it will terminate
   *         the program.
   */
  void run() override {
    // Initialize the socket for the client
    struct sockaddr_in serv;

    this->is_running_ = true;

    assert((this->server_fd_ = socket(AF_INET, SOCK_STREAM, 0)) >= 0);
    serv.sin_family = AF_INET;
    serv.sin_port = htons(this->server_port_num_);

    // Convert IP address from text to binary form
    assert(inet_pton(AF_INET, this->server_ip_addr_->c_str(), &serv.sin_addr) >
           0);
    assert(connect(this->server_fd_, (struct sockaddr *)&serv, sizeof(serv)) >=
           0);

    // Give some time for the server to finish establishing the connection
    // with this new client.
    sleep(5000); // 5 seconds

    this->init();

    while (this->continue_running_) {
      // Re-initialize the file descriptor set to prepare the polling
      FD_ZERO(&this->fd_selector_);
      FD_SET(this->server_fd_, &this->fd_selector_);

      // Poll for activity from the socket
      // TODO: Make the polling timeout be configurable
      struct timeval poll_receive_timeout;
      poll_receive_timeout.tv_sec = 0;
      poll_receive_timeout.tv_usec = 100;
      int activity = select(this->server_fd_ + 1, &this->fd_selector_, NULL,
                            NULL, &poll_receive_timeout);
      if (activity != 0) {
        // The activity is on this client. Read from it. It's okay to read
        // directly because it is the only one in the file descriptor selector.
        int bytes_read = read(this->server_fd_, this->receive_buffer_,
                              this->max_receive_size_);
        if (bytes_read == 0) {
          // This is a closing connection.
          this->handle_closing_connection();
          this->continue_running_ = false;
          break;
        } else if (bytes_read != -1) {
          // this is a received message. Call the callback function
          this->handle_incoming_message(this->receive_buffer_, bytes_read);
        }
      }

      if (this->outgoing_message_queue_->getSize() > 0) {
        // A message is queued up. Send it.
        this->outgoing_queue_lock_->lock();
        OutgoingMessage_ *msg_info = dynamic_cast<OutgoingMessage_ *>(
            this->outgoing_message_queue_->dequeue());
        this->outgoing_queue_lock_->unlock();

        // The queue should only ever contain valid outgoing message
        // information.
        assert(msg_info != nullptr);

        // Send the message
        send(this->server_fd_, msg_info->sending_buffer, msg_info->message_size,
             0);

        // No need for the sending buffer anymore. Clear it out now.
        delete msg_info;
      }

      this->run_client();
    }

    // Close the client
    close(this->server_fd_);
    this->server_fd_ = 0;

    this->is_running_ = false;
  }

  /**
   * Closes the client. This function can be called by other processes, or it
   * can be called within the same class itself.
   *
   * NOTE: This function will not immediately close the server. It will
   * finish one iteration of its run loop before it fully closes.
   */
  void close_client() {
    if (this->is_running()) {
      this->continue_running_ = false;
      this->join();
    }
  }

  /**
   * Determines if the server is running.
   * @return True if the server is running, false if otherwise.
   */
  bool is_running() { return this->is_running_; }

  /**
   * Sends a message to a client that is connected to the server.
   *
   * If the message buffer provided is null, the server will treat it as if
   * there is no message to send.
   * @param message An array of bytes containing the message with the
   *                specified size. This message should be dynamically
   *                allocated, and once it is passed into the function, the
   *                server will delete/free that memory once it has
   *                successfully sent the message.
   * @param bytes How many bytes the message is.
   * @return True if the message has successfully been queued to be sent.
   *         False if not (i.e., the message is null, etc.)
   */
  bool send_message(unsigned char *message, size_t bytes) {
    if (message == nullptr) {
      return false;
    }

    // Queue this message up.
    OutgoingMessage_ *msg_info = new OutgoingMessage_(0, message, bytes);
    this->outgoing_queue_lock_->lock();
    this->outgoing_message_queue_->enqueue(msg_info);
    this->outgoing_queue_lock_->unlock();
    return true;
  }

  /**
   * Gets the maximum number of bytes the server can receive from a client.
   * @return The maximum number of bytes a received message can be.
   */
  size_t get_max_receive_size() { return this->max_receive_size_; }
};

/**
 * Name: Snowy Chen, Joe Song
 * Date: 21 February 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::CwC

#pragma once

#include <queue>
#include "thread.h"

class OutgoingMessage_;

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

  /**
   * Constructs a server. All inheriting classes should be calling one of the
   * constructors upon their construction as well, in order for the concrete
   * functions in this abstract class to function as expected.
   *
   * NOTE: This instance supports a maximum of 1 client.
   * @param ip_addr The IP address of the server. This string will be acquired.
   * @param port_num The port number of the server.
   */
  Server(String *ip_addr, int port_num);

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
         size_t max_receive_size);

  /**
   * Deconstructs a server. The server should NOT be deconstructed shortly
   * after start() gets called--it should be allowed at least 5 seconds to
   * run. The deconstructor also closes the server if it is running.
   */
  ~Server() override;

  /**
   * An abstract function that a server implementation should implement. This
   * function will be called right before the infinite loop in start_server(),
   * but after all other generic server properties have completed
   * initialization.
   *
   * NOTE: This function should only be implemented, but never called by any
   * code external to the original Server class.
   */
  virtual void init() {}

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
  virtual void run_server() {}

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
  virtual void handle_incoming_message(size_t client_id, unsigned char *buffer,
                                       size_t num_bytes) {}

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
                                          int port_num) {}

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
  virtual void handle_closing_connection(size_t client_id) {}

  /**
   * An abstract function that the server implementation should implement.
   * This function will handle when the server is shutting down. This will be
   * called before the server has closed all of the sockets.
   */
  virtual void handle_shutdown() {}

  /**
   * Runs the server in an infinite loop. The server should be started by
   * calling the thread's start() function.
   * @throws If the server cannot start up properly, it will terminate the
   *         program.
   */
  void run() override;

  /**
   * Closes the server. This function can be called by other processes, or it
   * can be called within the same class itself.
   *
   * NOTE: This function will not immediately close the server. It will
   * finish one iteration of its run loop before it fully closes.
   */
  void close_server();

  /**
   * Determines if the server is running.
   * @return True if the server is running, false if otherwise.
   */
  bool running() { return this->is_running; }

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
  bool send_message(size_t client_id, unsigned char *message, size_t bytes);

  /**
   * Determines if the provided client id is connected.
   * @param client_id The client id to query
   * @return True if there is a client connected at this id. False if
   * otherwise (including when the client id is out of bounds).
   */
  bool is_client_connected(size_t client_id);

  /**
   * Initiates a connection to a specified IP address and port number. It
   * will attach this connection to the specified client id.
   * @param client_id The unique client id.
   * @param ip_addr The IP address to connect to.
   * @param port_num The port number that we want to connect to.
   * @return True if the connection was successful. False if otherwise (i.e.,
   * the client id has already been connected to another client).
   */
  bool initiate_connection(size_t client_id, String *ip_addr, int port_num);

  /**
   * Forcibly close a connection with a client at the specified id.
   * @param client_id The unique id of the client to close connection with.
   */
  void close_connection(size_t client_id);

  /****************************
   * Configuration functions
   ****************************/

  size_t get_max_clients() { return this->max_clients; }
  size_t get_max_receive_size() { return this->max_receive_size; }
  String *get_ip_addr() { return this->ip_addr; }
  size_t get_port_num() { return this->port_num; }

private:
  bool continue_running;
  bool is_running;

  String *ip_addr;
  int port_num;
  int server_fd;       // File descriptor of the server
  int *clients_sockets; // Array of file descriptors for all the clients. 0
  // if it is not used
  fd_set socket_set{}; // A socket set selector to manage multiple clients
  int max_socket_descriptor; // The current maximum file descriptor.
  Lock loop_lock;           // Locks whenever a server loop is running

  unsigned char *receive_buffer;
  std::queue<OutgoingMessage_ *> outgoing_message_queue;
  Lock outgoing_queue_lock;

  // Configuration fields
  size_t max_clients;
  size_t max_receive_size;

  /**
   * Private function to initialize the server. This function should never be
   * called externally.
   */
  void initialize_server();

  /**
   * Private function handling new incoming connection.
   */
  void add_new_connection();

  /**
   * Private function that manages activity on the client connections.
   */
  void manage_client_connections();
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

  /**
   * Constructs a client. All inheriting classes should be calling this
   * constructor upon their construction as well, in order for the concrete
   * functions in this abstract class to function as expected.
   * @param server_ip_addr The server IP address. The string is acquired.
   * @param server_port_num The port number of the server.
   */
  Client(String *server_ip_addr, int server_port_num);

  /**
   * Constructs a client. All inheriting classes should be calling one of the
   * constructors upon their constructions as well in order for the concrete
   * functions in this abstract class to function as expected.
   * @param server_ip_addr The server IP address. The string is acquired.
   * @param server_port_num The port number of the server.
   * @param max_receive_size The maximum number of bytes this server can
   *                         receive.
   */
  Client(String *server_ip_addr, int server_port_num, size_t max_receive_size);

  /**
   * Deconstructs a client. The client should NOT be deconstructed shortly
   * after start() gets called--it should be allowed at least 10 seconds to
   * run. The deconstructor also closes the client if it is running.
   */
  ~Client() override;

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
   * An abstract function that the client implementation should implement.
   * This function will handle when the client is shutting down. This will be
   * called before the client has closed its connection to the server.
   */
  virtual void handle_shutdown() {}

  /**
   * Starts the client. It will run on an infinite loop, calling the
   * abstracted function run(), which should be implemented by children
   * classes, upon each loop iteration, until the function close_client() gets
   * called.
   * @throws If the client cannot start up properly, and it cannot connect
   *         the spcified server at the provided IP address, it will terminate
   *         the program.
   */
  void run() override;

  /**
   * Closes the client. This function can be called by other processes, but
   * not within the class itself. It will wait for the client to fully close.
   */
  void close_client();

  /**
   * Determines if the server is running.
   * @return True if the server is running, false if otherwise.
   */
  bool running() { return this->is_running; }

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
  bool send_message(unsigned char *message, size_t bytes);

  size_t get_max_receive_size() { return this->max_receive_size; }

private:
  bool continue_running;
  bool is_running;

  String *server_ip_addr;
  int server_port_num;
  int server_fd;
  fd_set fd_selector{};

  unsigned char *receive_buffer;
  std::queue<OutgoingMessage_ *> outgoing_message_queue;
  Lock outgoing_queue_lock;

  // Configuration fields
  size_t max_receive_size;
};

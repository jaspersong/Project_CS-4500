/**
 * Name: Snowy Chen, Joe Song
 * Date: 21 February 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::CwC

#pragma once

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
class MessageReceiver;

/**
 * An implementation of Server that multiple clients can register with. Once
 * the client has been connected to the server, it will then send clients the
 * addresses of all clients to each client (1 address per message).
 */
class Registrar : public Server {
public:
  /**
   * Constructs a server with a fixed number of maximum number of clients.
   * @param ip_addr The IP address of the server. This string will be acquired.
   * @param port_num The port number of the server.
   * @param max_num_clients The fixed number of maximum number of clients
   *        that the server can register at any given time.
   * @param received_message_manager The message manager that will handle
   *        receiving the messages
   */
  Registrar(String *ip_addr, int port_num, size_t max_num_clients,
            ReceivedMessageManager &received_message_manager);
  ~Registrar() override;

  void handle_incoming_message(size_t client_id, unsigned char *buffer,
                               size_t num_bytes) override;
  void handle_incoming_connection(size_t new_client_id, String *addr,
                                  int port_num) override;
  void handle_closing_connection(size_t client_id) override;

private:
  // An array of client information. This array will have all the client
  // information in a contiguous fashion.
  Directory *client_dir;

  MessageReceiver *message_receiver;
  ReceivedMessageManager *received_msg_manager;

  /**
   * Helper function that broadcasts the updated client directory to all of
   * the connected and registered nodes to this server.
   */
  void broadcast_client_directory();
};

/**
 * An implementation of Client that connects to a server and receives
 * addresses (1 address per incoming message) of other clients that have
 * connected to that server. Then the client can sustain direct communication
 * with the other clients.
 */
class Node : public Client {
public:
  /**
   * Constructs a client.
   * @param server_ip_addr The server IP address. The string is acquired.
   * @param server_port_num The port number of the server.
   * @param ip_addr The IP address of the client. The string is acquired
   * @param port_num The port number of this client.
   * @param fixed_num_clients The fixed number of clients that the client can
   *        directly communicate with after receiving the addresses from the
   *        server.
   * @param msg_manager The message manager that will handle receiving
   *        the messages from server and node.
   * @param dm_msg_manager The message manager that will handle receiving
   *        the messages going directly between nodes
   */
  Node(String *server_ip_addr, int server_port_num, String *ip_addr,
       int port_num, size_t fixed_num_clients,
       ReceivedMessageManager &msg_manager,
       ReceivedMessageManager &dm_msg_manager);
  ~Node() override;

  void init() override;
  void handle_shutdown() override;
  void handle_incoming_message(unsigned char *buffer,
                               size_t num_bytes) override;

  /**
   * Sends a direct message to the client at the specified client id.
   * @param client_id The unique client id.
   * @param msg The message that will be sent to the specified client.
   * @return True if the message has successfully been queued. False if
   * otherwise.
   */
  bool send_direct_message(size_t client_id, Message &msg);

  /**
   * Gets the id of the node. If the node has not been assigned an id, then
   * it will wait until it is available first.
   * @return The id of the node
   */
  size_t get_node_id_with_wait();

private:
  class ClientDMManager;

  ClientDMManager *dm_manager;
  MessageReceiver *message_receiver;
  ReceivedMessageManager *received_msg_manager;

  size_t id;
  Lock enum_signal;
};

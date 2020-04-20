/**
 * Name: Snowy Chen, Joe Song
 * Date: 19 April 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::Cpp

#pragma once

#include "thread.h"
#include <messages/serial.h>
#include <queue>

/**
 * A socket network layer. It is able to initiate connections to known IP
 * address-port number pairs, and have others connect to this.
 */
class Network : public Thread {
public:
  /**
   * Constructs a socket network.
   * @param The node id
   * @param ip_addr The IP address that the network will listen on for
   * incoming connections and incoming messages.
   * @param port_num The port number
   * @param max_clients The maximum number of connections that the network
   * can connect to at any single moment in time.
   * @param max_receive_size The maximum number of bytes this server can
   * receive in an incoming message.
   */
  Network(size_t id, const char *ip_addr, int port_num, size_t max_connections);
  ~Network() override;

  String *get_ip_addr() { return &this->ip_addr; }
  int get_port_num() { return this->port_num; }
  virtual size_t get_id() { return this->id; }
  void close_network();

  bool is_id_connected(size_t connection_id);
  void initiate_connection(size_t connection_id, String *connection_ip_addr,
                           int connection_port_num);
  void close_connection(size_t connection_id);

  void send_message(size_t target_id, Message &msg);
  void broadcast_message(Message &msg);

  // Callback functions
  virtual void handle_initialization() {}
  virtual void handle_closing_connection(size_t connection_id) {}
  // The passed in message will be owned by the function, so the
  // responsibility to deallocate the message is on the function itself.
  virtual void handle_incoming_message(size_t connection_id, Message *msg) {
    delete msg;
  }

protected:
  size_t id;

  void println(StrBuff &msg);

private:
  bool continue_running;
  bool is_running;

  String ip_addr;
  int port_num;
  int listening_socket;

  size_t max_connections;
  int *connection_sockets;
  Lock connection_lock;

  unsigned char *msg_buffer;
  size_t msg_buffer_index;
  size_t msg_buffer_size;

  Lock outgoing_msg_lock;

  void run() override;
  void initialize_listening_socket();
  void add_new_connection();
  Message *read_socket(int connection_id);
};

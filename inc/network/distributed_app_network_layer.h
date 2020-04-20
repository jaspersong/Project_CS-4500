/**
 * Name: Snowy Chen, Joe Song
 * Date: 19 April 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::Cpp

#pragma once

#include "network.h"
#include "recv_msg_manager.h"

class Registrar : public Network {
public:
  Registrar(const char *ip_addr, int port_num, size_t max_connections,
      ReceivedMessageManager *received_message_manager);
  ~Registrar() override;

  void handle_closing_connection(size_t connection_id) override;
  void handle_incoming_message(size_t connection_id, Message *msg) override;
  void wait_for_all_connected() override;

private:
  Directory *directory;
  Lock directory_lock;
  Lock full_connection_signal;
  bool at_full_connection;

  ReceivedMessageManager *received_msg_manager;
};

class Node : public Network {
public:
  Node(const char *registrar_addr, int registrar_port_num,
      const char *listener_ip_addr, int listener_port_num,
      size_t max_connections, ReceivedMessageManager *received_message_manager);
  ~Node() override;

  size_t get_id() override;

  void handle_initialization() override;
  void handle_incoming_message(size_t connection_id, Message *msg) override;
  void wait_for_all_connected() override;

private:
  String registrar_ip_addr;
  int registrar_port_num;

  Directory *directory;
  Lock directory_lock;
  Lock enum_signal;
  Lock full_connection_signal;
  bool at_full_connection;

  ReceivedMessageManager *received_msg_manager;
};

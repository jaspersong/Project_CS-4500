/**
 * Name: Snowy Chen, Joe Song
 * Date: 21 February 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::CwC

#pragma once

#include "network.h"

/**
 * A dummy class that simply echoes the ASCII messages that it receives onto
 * the stdout. This class is primarily used to test out the abstract Server
 * class in order to ensure that the basic connectivity between Server and
 * Client classes work properly.
 */
class EchoServer : public Server {
public:
  EchoServer(String *ip_addr, int port_num) : Server(ip_addr, port_num, 3,
      1024) {}

  void handle_incoming_message(size_t client_id,
                               unsigned char *buffer,
                               size_t num_bytes) override {
    // Print out the message to stdout
    printf("Received message from client_id %zu: %s\n", client_id, buffer);

    // Now send a message in response
    const char * reply = "Received client message: ";
    size_t reply_size = strlen(reply) + num_bytes;
    char * msg = new char[reply_size];
    strcpy(msg, reply);
    strcat(msg, reinterpret_cast<const char *>(buffer));
    this->send_message(client_id, reinterpret_cast<unsigned char *>(msg),
                       reply_size);
    printf("Sent response \"%s\".\n", msg);
  }

  void handle_incoming_connection(size_t new_client_id, String *
    addr, int port_num) override {
    printf("New incoming connection from client_id %zu, address %s, port "
           "number %d\n",
        new_client_id, addr->c_str(), port_num);
  }

  void handle_closing_connection(size_t client_id) override {
    printf("Closed connection from client_id %zu\n", client_id);
  }
};

/**
 * A dummy class that simply echoes the ASCII messages that it receives from
 * the server onto stdout. This class is primarily used to test out the
 * abstract Client class in order to ensure taht the basic connectivity
 * between Server and Client classes work properly.
 */
class EchoClient : public Client {
public:
  EchoClient(String *server_ip_addr, int server_port_num) :
    Client(server_ip_addr, server_port_num) {}

  void handle_incoming_message(unsigned char *buffer,
                               size_t num_bytes) override {
    // Print out the message to stdout
    printf("Received message from server: %s\n", buffer);
  }

  void handle_closing_connection() override {
    printf("Server has closed the connection.\n");
    printf("Closing the client as well.\n");
  }
};

/**
 * Name: Snowy Chen, Joe Song
 * Date: 21 March 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::Cpp

#include "distributed_app_network_layer.h"

Registrar::Registrar(const char *ip_addr, int port_num, size_t max_connections,
                     ReceivedMessageManager *received_message_manager)
    : Network(0, ip_addr, port_num, max_connections) {
  this->directory = new Directory(max_connections);
  this->directory->add_client(0, this->get_ip_addr(), this->get_port_num());

  this->received_msg_manager = received_message_manager;
}

Registrar::~Registrar() { delete this->directory; }

void Registrar::handle_closing_connection(size_t connection_id) {
  printf("%zu: Connection id %zu closed connection\n", this->get_id(),
         connection_id);
  this->directory_lock.lock();
  this->directory->remove_client(connection_id);
  this->broadcast_message(*this->directory);
  this->directory_lock.unlock();
}

void Registrar::handle_incoming_message(size_t connection_id, Message *msg) {
  bool dont_delete = false;

  // Only register case uses these.
  Register *reg_message = nullptr;
  String *ip_addr = nullptr;

  switch (msg->get_message_kind()) {
  case MsgKind::Put:
    dont_delete = this->received_msg_manager->handle_put(msg->as_put());
    break;
  case MsgKind::Reply:
    dont_delete = this->received_msg_manager->handle_reply(msg->as_reply());
    break;
  case MsgKind::WaitAndGet:
    dont_delete =
        this->received_msg_manager->handle_waitandget(msg->as_waitandget());
    break;
  case MsgKind::Status:
    dont_delete = this->received_msg_manager->handle_status(msg->as_status());
    break;
  case MsgKind::Register:
    // Add this client to the directory
    this->println(
        StrBuff().c("Received Register message from id ").c(connection_id));

    // Register the client id to the directory
    reg_message = msg->as_register();
    ip_addr = msg->as_register()->get_ip_addr();
    this->println(StrBuff()
                      .c("Registered IP address ")
                      .c(ip_addr->c_str())
                      .c(", port number ")
                      .c(reg_message->get_port_num()));

    this->directory_lock.lock();
    this->directory->add_client(connection_id, ip_addr,
                                reg_message->get_port_num());

    // Broadcast the updated registrar
    this->broadcast_message(*this->directory);
    this->directory_lock.unlock();

    delete ip_addr;
    break;
  case MsgKind::Directory:
    this->println(StrBuff()
                      .c("Received Directory message from id ")
                      .c(connection_id)
                      .c(". Ignoring."));
    break;
  default:
    this->println(StrBuff()
                      .c("Received invalid message from id ")
                      .c(connection_id)
                      .c(". Ignoring."));
    break;
  }

  if (!dont_delete) {
    delete msg;
  }
}

Node::Node(const char *registrar_addr, int registrar_port_num,
           const char *listener_ip_addr, int listener_port_num,
           size_t max_connections,
           ReceivedMessageManager *received_message_manager)
    : Network(-1, listener_ip_addr, listener_port_num, max_connections),
      registrar_ip_addr(registrar_addr) {
  this->registrar_port_num = registrar_port_num;
  this->directory = nullptr;
  this->received_msg_manager = received_message_manager;
}

Node::~Node() { delete this->directory; }

void Node::handle_initialization() {
  // Connect to the registrar
  this->initiate_connection(0, &this->registrar_ip_addr,
                            this->registrar_port_num);

  // Set up the register message and send it to the registrar
  Register reg(this->get_ip_addr(), this->get_port_num());
  this->send_message(0, reg);
}

void Node::handle_incoming_message(size_t connection_id, Message *msg) {
  bool dont_delete = false;

  // Only the directory case uses these
  Directory *dir_message = nullptr;

  switch (msg->get_message_kind()) {
  case MsgKind::Put:
    dont_delete = this->received_msg_manager->handle_put(msg->as_put());
    break;
  case MsgKind::Reply:
    dont_delete = this->received_msg_manager->handle_reply(msg->as_reply());
    break;
  case MsgKind::WaitAndGet:
    dont_delete =
        this->received_msg_manager->handle_waitandget(msg->as_waitandget());
    break;
  case MsgKind::Status:
    dont_delete = this->received_msg_manager->handle_status(msg->as_status());
    break;
  case MsgKind::Register:
    this->println(StrBuff()
                      .c("Received Register message from id ")
                      .c(connection_id)
                      .c(". Ignoring."));
    break;
  case MsgKind::Directory:
    // Replace the current directory with the updated directory
    this->println(StrBuff().c("Received Directory from registrar"));
    dir_message = msg->as_directory();

    // Update the directory
    this->directory_lock.lock();
    // Close sockets for clients that no longer are connected
    for (size_t i = 0; i < dir_message->get_max_num_clients(); i++) {
      if (!dir_message->is_client_connected(i)) {
        this->close_connection(i);
      }
    }
    delete this->directory;
    this->directory = dir_message;
    this->directory_lock.unlock();

    // Pull the node id from the directory message as the enumeration, if
    // this is the first directory message we have received.
    if (this->id == -1) { // This means that the node id hasn't been assigned.
      this->id = msg->get_target_id();

      this->println(StrBuff().c("Assigned node id ").c(this->id));
      this->enum_signal.notify_all();
    } else {
      // Initiate the connection to all of the currently connected clients to
      // the directory. We can iterate from this id (exclusive) to
      // the last id because we know that the registrar will assign node ids in
      // the order that connect to the registrar
      for (size_t i = this->id + 1; (i < dir_message->get_max_num_clients()) &&
                                    (dir_message->is_client_connected(i));
           i++) {
        this->initiate_connection(i, dir_message->get_client_ip_addr(i),
                                  dir_message->get_client_port_num(i));
      }
    }

    // Make sure not to delete this
    dont_delete = true;
    break;
  default:
    // Invalid message type. Do nothing and just ignore it.
    printf("Received invalid message from server\n");
    break;
  }

  if (!dont_delete) {
    delete msg;
  }
}

size_t Node::get_id() {
  if (this->id == -1) {
    // This means that the id has not been found yet. Wait for it to be
    // available
    this->enum_signal.wait();
  }

  return this->id;
}

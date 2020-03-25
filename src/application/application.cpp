/**
 * Name: Snowy Chen, Joe Song
 * Date: 21 March 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::Cpp

#include "application.h"

Application::Application(size_t num_nodes) {
  assert(num_nodes > 0);

  this->kv = new KeyValueStore(num_nodes);
  this->running = false;
}

void Application::main() { assert(false); }

size_t Application::get_node_id() { return this->kv->get_home_id(); }

LocalNetworkMessageManager *Application::connect_local(size_t node_id) {
  assert(!this->running);
  return this->kv->connect_local(node_id);
}

void Application::register_local(LocalNetworkMessageManager *msg_manager) {
  assert(!this->running);
  assert(msg_manager != nullptr);
  this->kv->register_local(msg_manager);
}

void Application::connect_network(Node& node) {
  assert(!this->running);
  this->kv->connect_network(node);
}

void Application::run() {
  this->running = true;
  assert(this->kv->verify_distributed_layer());
  this->main();
  this->running = false;
}

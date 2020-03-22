/**
 * Name: Snowy Chen, Joe Song
 * Date: 21 March 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::Cpp

#include "application.h"

Application::Application(size_t node_id) {
  this->kv = new KeyValueStore(node_id);
  this->node_id = node_id;
}

void Application::run() { assert(false); }

size_t Application::get_node_id() { return this->node_id; }

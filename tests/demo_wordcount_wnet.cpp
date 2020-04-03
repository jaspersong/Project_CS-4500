/**
 * Name: Snowy Chen, Joe Song
 * Date: 23 March 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::Cpp

#include <unistd.h>
#include "word_count_app.h"
#include "stdout_msg_manager.h"
#include "networked_msg_manager.h"

static const size_t NUM_COUNTERS = 2;

int main(int argc, char **argv) {
  assert(argc == 2);
  String file_name(argv[1]);

  WordCount main_node(file_name);
  WordCount *counters[NUM_COUNTERS];
  for (auto & counter : counters) {
    counter = new WordCount(file_name);
  }

  RealNetworkMessageManager *main_network_manager;
  RealNetworkMessageManager *counter_network_managers[NUM_COUNTERS];

  // Register each node for the real network
  main_network_manager = main_node.connect_network();
  for (size_t i = 0; i < NUM_COUNTERS; i++) {
    counter_network_managers[i] = counters[i]->connect_network();
  }

  // Now connect each node to its network layer
  StdoutMessageManager msg_manager(0);
  // TODO: For some reason, if the max number of clients is just the amount
  //  needed, the registrar refuses to connect the last node
  Registrar server(new String("127.0.0.1"), 1234,
                   NUM_COUNTERS + 2, msg_manager);
  Node prod_node(new String("127.0.0.1"), 1234,
                 new String("127.0.0.1"), 1235, NUM_COUNTERS + 1,
                 *main_network_manager, *main_network_manager);
  Node *counter_nodes[NUM_COUNTERS];
  for (size_t i = 0; i < NUM_COUNTERS; i++) {
    counter_nodes[i] = new Node(new String("127.0.0.1"), 1234,
                                new String("127.0.0.1"),
                                1236 + static_cast<int>(i),
                                NUM_COUNTERS + 1,
                                *counter_network_managers[i],
                                *counter_network_managers[i]);
  }

  // Start up all of the network layers
  server.start();
  sleep(10); // Give the registrar time to start up
  prod_node.start();
  for (auto & counter_node : counter_nodes) {
    sleep(10);
    counter_node->start();
  }
  sleep(10);

  // Start up all of the applications
  main_node.start();
  for (auto & counter : counters) {
    counter->start();
  }

  // Now wait for them all to finish
  main_node.join();
  for (auto & counter : counters) {
    counter->join();
  }

  // Now close all of the network layers
  for (auto & counter_node : counter_nodes) {
    counter_node->close_client();
  }
  prod_node.close_client();
  sleep(5);
  server.close_server();

  for (auto & counter_node : counter_nodes) {
    delete counter_node;
  }
  delete main_network_manager;
  for (size_t i = 0; i < NUM_COUNTERS; i++) {
    delete counters[i];
    delete counter_network_managers[i];
  }

  return 0;
}

/**
 * Name: Snowy Chen, Joe Song
 * Date: 23 March 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::Cpp

#include "word_count_app.h"
#include "local_network_msg_manager.h"

static const size_t NUM_COUNTERS = 7;

int main(int argc, char **argv) {
  WordCount main_node;
  WordCount counters[NUM_COUNTERS];

  LocalNetworkMessageManager *main_network_manager;
  LocalNetworkMessageManager *counter_network_managers[NUM_COUNTERS];

  main_network_manager = main_node.connect_local(0);
  for (size_t i = 0; i < NUM_COUNTERS; i++) {
    counter_network_managers[i] = counters[i].connect_local(i + 1);

    // Connect the counter's network manager to the main network manager
    main_node.register_local(counter_network_managers[i]);
  }

  // Now connect all of the counters to each other to establish direct
  // communication
  for (size_t i = 0; i < NUM_COUNTERS; i++) {
    counters[i].register_local(main_network_manager);
    for (size_t j = 0; j < NUM_COUNTERS; j++) {
      if (i != j) {
        counters[i].register_local(counter_network_managers[j]);
      }
    }
  }

  // Start up all of the applications
  main_node.start();
  for (auto & counter : counters) {
    counter.start();
  }

  // Now wait for them all to finish
  main_node.join();
  for (auto & counter : counters) {
    counter.join();
  }

  delete main_network_manager;
  for (auto & counter_network_manager : counter_network_managers) {
    delete counter_network_manager;
  }

  return 0;
}

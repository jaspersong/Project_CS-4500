/**
 * Name: Snowy Chen, Joe Song
 * Date: 23 March 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::Cpp

#include <unistd.h>
#include "demo_app.h"
#include "stdout_msg_manager.h"
#include "networked_msg_manager.h"

int main(int argc, char **argv) {
  Demo producer;
  Demo counter;
  Demo summarizer;

  RealNetworkMessageManager *prod_manager = producer.connect_network();
  RealNetworkMessageManager *counter_manager = counter.connect_network();
  RealNetworkMessageManager *summarizer_manager = summarizer.connect_network();

  // Set up the network layer
  Registrar prod_node("127.0.0.1", 1234, 3, prod_manager);
  Node counter_node("127.0.0.1", 1234, "127.0.0.1", 1235, 3, counter_manager);
  Node summarizer_node("127.0.0.1", 1234, "127.0.0.1", 1236, 3,
      summarizer_manager);

  // Start up the registrar and the network layers one by one
  prod_node.start();
  sleep(5);
  counter_node.start();
  sleep(5);
  summarizer_node.start();
  sleep(5);

  // Now start up all of the node applications
  producer.start();
  counter.start();
  summarizer.start();

  // Wait for all of the node applications to finish
  producer.join();
  counter.join();
  summarizer.join();

  // Now close everything one by one
  summarizer_node.close_network();
  counter_node.close_network();
  prod_node.close_network();

  delete prod_manager;
  delete counter_manager;
  delete summarizer_manager;

  return 0;
}

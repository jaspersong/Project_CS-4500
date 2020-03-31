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
  StdoutMessageManager msg_manager(0);
  Registrar server(new String("127.0.0.1"), 1234,
      4, msg_manager);
  Node prod_node(new String("127.0.0.1"), 1234,
      new String("127.0.0.1"), 1235, 3,
      *prod_manager, *prod_manager);
  Node counter_node(new String("127.0.0.1"), 1234,
      new String("127.0.0.1"), 1236, 3,
      *counter_manager, *counter_manager);
  Node summarizer_node(new String("127.0.0.1"), 1234,
                    new String("127.0.0.1"), 1237, 3,
                    *summarizer_manager, *summarizer_manager);

  // Start up the registrar and the network layers one by one
  server.start();
  sleep(10); // Give the registrar time to start up
  prod_node.start();
  sleep(10); // Give node 0 time to start up
  counter_node.start();
  sleep(10);
  summarizer_node.start();

  // Now start up all of the node applications
  producer.start();
  counter.start();
  summarizer.start();

  // Wait for all of the node applications to finish
  producer.join();
  counter.join();
  summarizer.join();

  // Now close everthing one by one
  summarizer_node.close_client();
  counter_node.close_client();
  prod_node.close_client();
  sleep(5);
  server.close_server();

  delete prod_manager;
  delete counter_manager;
  delete summarizer_manager;

  return 0;
}

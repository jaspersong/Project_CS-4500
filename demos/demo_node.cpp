/**
 * Name: Snowy Chen, Joe Song
 * Date: 21 February 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::CwC

#include "distributed_app_network_layer.h"
#include "stdout_msg_manager.h"
#include <unistd.h>

int main(int argc, char **argv) {
  // Creates a server that sends all strings provided as arguments to the
  // node that connects to it.
  StdoutMessageManager msg_manager0(1);
  StdoutMessageManager msg_manager1(2);
  Node node1("127.0.0.1", 1234, "127.0.0.1", 1235, 3, &msg_manager0);
  Node node2("127.0.0.1", 1234, "127.0.0.1", 1236, 3, &msg_manager0);

  // Run the server on localhost with port 1234
  node1.start();
  node2.start();
  sleep(5);

  // Queue up some messages from node 1 to node 0.
  for (int i = 0; i < argc; i++) {
    String status_message(argv[i]);
    Status status(status_message);

    printf("Sending status message: %s\n", argv[i]);
    node1.send_message(0, status);
  }

  // Queue up some more messages from node 2
  for (int i = 0; i < argc; i++) {
    String status_message(argv[i]);
    Status status(status_message);

    printf("Sending status message: %s\n", argv[i]);
    node2.send_message(0, status);
    node2.send_message(1, status);
  }

  sleep(15);
  node1.close_network();

  return 0;
}

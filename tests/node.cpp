/**
 * Name: Snowy Chen, Joe Song
 * Date: 21 February 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::CwC

#include <unistd.h>
#include "distributed_app.h"
#include "stdout_msg_manager.h"

int main(int argc, char **argv) {
  // Creates a server that sends all strings provided as arguments to the
  // client that connects to it.
  StdoutMessageManager msg_manager0(10);
  StdoutMessageManager msg_manager1(20);
  StdoutMessageManager dm_msg_manager0(11);
  StdoutMessageManager dm_msg_manager1(21);
  Node *client0 =
      new Node(new String("127.0.0.1"), 1234, new String("127.0.0.1"), 1235, 4,
               msg_manager0, dm_msg_manager0);
  Node *client1 =
      new Node(new String("127.0.0.1"), 1234, new String("127.0.0.1"), 1236, 4,
               msg_manager1, dm_msg_manager1);

  // Run the server on localhost with port 1234
  client0->start();
  sleep(10); // Let the client0 be assigned the node id 0 before starting node 1
  client1->start();
  sleep(10); // Let client1 be assigned the node id 1 and establish direct
  // communication with other nodes.

  // Queue up some messages from client 1 to client 0.
  for (int i = 0; i < argc; i++) {
    String status_message(argv[i]);
    Status status(status_message);

    printf("Sending status message: %s\n", argv[i]);
    client1->send_direct_message(0, status);
  }

  sleep(15);
  client0->close_client();
  client1->close_client();
  delete client0;
  delete client1;

  return 0;
}

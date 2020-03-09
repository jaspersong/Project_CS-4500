/**
 * Name: Snowy Chen, Joe Song
 * Date: 21 February 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::CwC

#include "distributed_app.h"

int main(int argc, char **argv) {
  // Creates a server that sends all strings provided as arguments to the
  // client that connects to it.
  Node *client0 = new Node(new String("127.0.0.1"), 1234, new
    String("127.0.0.1"), 1235,4);
  Node *client1 = new Node(new String("127.0.0.1"), 1234, new
      String("127.0.0.1"), 1236, 4);

  // Run the server on localhost with port 1234
  client0->start();
  client1->start();

  // Queue up some messages from client 1 to client 0.
  while (!client1->initiate_direct_message_connection(0)) {
    sleep(1);
  }
  for (int i = 0; i < argc; i++) {
    String *status_message = new String(argv[i]);
    Status *status = new Status(*status_message);

    size_t message_size = 0;
    unsigned char * message = status->serialize(message_size);

    client1->send_direct_message(0, message, message_size);
  }

  sleep(15);
  client0->close_client();
  client1->close_client();
  delete client0;
  delete client1;

  return 0;
}

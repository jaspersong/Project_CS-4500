/**
 * Name: Snowy Chen, Joe Song
 * Date: 21 February 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::CwC

#include "network.h"
#include "echo_network.h"

int main(int argc, char **argv) {
  // Creates a server that sends all strings provided as arguments to the
  // client that connects to it.
  EchoClient * client = new EchoClient(new String("127.0.0.1"), 1234);

  // Queue up all the messages
  for (int i = 0; i < argc; i++) {
    size_t message_size = strlen(argv[i]);
    unsigned char * message = new unsigned char[message_size];
    strcpy(reinterpret_cast<char *>(message), argv[i]);
    client->send_message(message, message_size + 1);
  }

  client->start();

  sleep(10);
  client->close_client();
  delete client;

  return 0;
}

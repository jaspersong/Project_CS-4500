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
  StdoutMessageManager msg_manager(0);
  Registrar *server =
      new Registrar(new String("127.0.0.1"), 1234, 4, msg_manager);

  // Run the server on localhost with port 1234
  server->start();

  sleep(120);
  server->close_server();
  delete server;

  return 0;
}

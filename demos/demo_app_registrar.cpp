/**
 * Name: Snowy Chen, Joe Song
 * Date: 19 April 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::Cpp

#include "argparser.h"
#include "demo_app.h"
#include "socket_network_msg_manager.h"
#include <unistd.h>

int main(int argc, char **argv) {
  ArgParser args(ArgParser::ParseTypes::Registrar, argc, argv);

  Demo producer;
  SocketNetworkMessageManager *prod_manager = producer.connect_network();
  Registrar prod_node(args.get_listener_addr(), args.get_listener_port(),
      3, prod_manager);

  // Start up the registrar and the application
  prod_node.start();
  sleep(5);
  producer.start();

  // Wait until the app completes and that the network closes down
  prod_node.join();

  delete prod_manager;
  return 0;
}

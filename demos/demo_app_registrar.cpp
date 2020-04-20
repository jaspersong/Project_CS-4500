/**
 * Name: Snowy Chen, Joe Song
 * Date: 19 April 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::Cpp

#include <unistd.h>
#include "argparser.h"
#include "demo_app.h"
#include "networked_msg_manager.h"

int main(int argc, char **argv) {
  ArgParser args(ArgParser::ParseTypes::Registrar, argc, argv);

  Demo producer;
  RealNetworkMessageManager *prod_manager = producer.connect_network();
  Registrar prod_node(args.get_listener_addr(), args.get_listener_port(),
      3, prod_manager);

  // Start up the registrar and the application
  prod_node.start();
  sleep(5);
  producer.start();

  // Keep it open to give the nodes the chance to do things
  sleep(480);

  // Close everything out
  producer.join();
  prod_node.close_network();
  delete prod_manager;
  return 0;
}

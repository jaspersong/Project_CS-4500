/**
 * Name: Snowy Chen, Joe Song
 * Date: 23 March 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::Cpp

#include <unistd.h>
#include "argparser.h"
#include "demo_app.h"
#include "networked_msg_manager.h"

int main(int argc, char **argv) {
  ArgParser args(ArgParser::ParseTypes::Node, argc, argv);

  Demo app;
  RealNetworkMessageManager *app_manager = app.connect_network();
  Node node(args.get_registrar_addr(), args.get_registrar_port(),
      args.get_listener_addr(), args.get_listener_port(),
      3, app_manager);

  // Start up the registrar and the application
  node.start();
  sleep(5);
  app.start();

  // Keep it open to give the nodes the chance to do things
  sleep(480);

  // Close everything out
  app.join();
  node.close_network();
  delete app_manager;
  return 0;
}

/**
 * Name: Snowy Chen, Joe Song
 * Date: 23 March 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::Cpp

#include "argparser.h"
#include "linus_app.h"
#include "socket_network_msg_manager.h"
#include <unistd.h>

int main(int argc, char **argv) {
  ArgParser args(ArgParser::ParseTypes::LinusNode, argc, argv);

  Linus app(args.get_num_expected_nodes());
  SocketNetworkMessageManager *app_manager = app.connect_network();
  Node node(args.get_registrar_addr(), args.get_registrar_port(),
            args.get_listener_addr(), args.get_listener_port(),
            args.get_num_expected_nodes(), app_manager);

  // Start up the registrar and the application
  node.start();
  sleep(5);
  app.start();

  // Wait until the app completes and that the node closes down
  app.detach();
  node.join();

  delete app_manager;
  return 0;
}

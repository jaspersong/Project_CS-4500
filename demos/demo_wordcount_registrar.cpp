/**
 * Name: Snowy Chen, Joe Song
 * Date: 23 March 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::Cpp

#include "argparser.h"
#include "socket_network_msg_manager.h"
#include "stdout_msg_manager.h"
#include "word_count_app.h"
#include <unistd.h>

int main(int argc, char **argv) {
  ArgParser args(ArgParser::ParseTypes::WordCountRegistrar, argc, argv);

  WordCount producer(args.get_num_expected_nodes(), args.get_wordcount_file());
  SocketNetworkMessageManager *prod_manager = producer.connect_network();
  Registrar prod_node(args.get_listener_addr(), args.get_listener_port(),
                      args.get_num_expected_nodes(), prod_manager);

  // Start up the registrar and the application
  prod_node.start();
  sleep(5);
  producer.start();

  // Wait until the app completes and that the network closes down
  producer.detach();
  prod_node.join();

  delete prod_manager;
  return 0;
}

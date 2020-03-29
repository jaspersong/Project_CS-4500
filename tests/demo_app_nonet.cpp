/**
 * Name: Snowy Chen, Joe Song
 * Date: 23 March 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::Cpp

#include "demo_app.h"
#include "local_network_msg_manager.h"

int main(int argc, char **argv) {
  Demo producer;
  Demo counter;
  Demo summarizer;

  LocalNetworkMessageManager *prod_manager = producer.connect_local(0);
  LocalNetworkMessageManager *counter_manager = counter.connect_local(1);
  LocalNetworkMessageManager *summarizer_manager = summarizer.connect_local(2);

  producer.register_local(counter_manager);
  producer.register_local(summarizer_manager);
  counter.register_local(prod_manager);
  counter.register_local(summarizer_manager);
  summarizer.register_local(prod_manager);
  summarizer.register_local(counter_manager);

  producer.start();
  counter.start();
  summarizer.start();

  producer.join();
  counter.join();
  summarizer.join();

  delete prod_manager;
  delete counter_manager;
  delete summarizer_manager;

  return 0;
}

/**
 * Name: Snowy Chen, Joe Song
 * Date: 23 March 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::Cpp

#include "demo_app.h"

int main(int argc, char **argv) {
  Demo producer;
  Demo counter;
  Demo summarizer;

  producer.connect_local(0);
  counter.connect_local(1);
  summarizer.connect_local(2);

  producer.add_local(counter);
  producer.add_local(summarizer);
  counter.add_local(producer);
  counter.add_local(summarizer);
  summarizer.add_local(producer);
  summarizer.add_local(counter);

  producer.start();
  counter.start();
  summarizer.start();

  producer.join();
  counter.join();
  summarizer.join();

  return 0;
}

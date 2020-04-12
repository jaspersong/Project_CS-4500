/**
 * Name: Snowy Chen, Joe Song
 * Date: 21 March 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::Cpp

#pragma once

#include "application.h"
#include "key.h"

class Demo : public Application {
public:
  /**
   * Constructs the demo application
   */
  explicit Demo();

  void main() override;

private:
  Key main_key = Key("main");
  Key verify = Key("verif");
  Key check = Key("ck");

  void producer();
  void counter();
  void summarizer();
};

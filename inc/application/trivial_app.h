/**
 * Name: Snowy Chen, Joe Song
 * Date: 21 March 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::Cpp

#pragma once

#include "application.h"

/**
 * A trivial implementation of an application that could be run on the eau2
 * system.
 */
class Trivial : public Application {
public:
  explicit Trivial() : Application(0){};

  void run() override;
};

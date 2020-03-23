/**
 * Name: Snowy Chen, Joe Song
 * Date: 21 March 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// lang::Cpp

#pragma once

#include "custom_object.h"
#include "custom_string.h"

#include "fielder.h"
#include "row.h"
#include "rower.h"

/**
 * An implementation of a Fielder that prints a row onto stdout in a SoR format.
 */
class PrintFielder : public Fielder {
public:
  explicit PrintFielder(Row &row);

  void start(size_t r) override;
  void accept(bool b) override;
  void accept(float f) override;
  void accept(int i) override;
  void accept(String *s) override;

private:
  Row *row;
};

/**
 * An implementation of a Rower that goes through a dataframe and prints the
 * values in a SoR format to stdout.
 */
class PrintRower : public Rower {
public:
  bool accept(Row &r) override;
};

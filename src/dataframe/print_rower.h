/**
 * Name: Snowy Chen, Joe Song
 * Date: 14 February 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

//lang::CwC

#pragma once

#include <stdlib.h>

#include "object.h"
#include "custom_string.h"

#include "fielder.h"
#include "row.h"
#include "rower.h"

/**
 * An implementation of a Fielder that prints a row onto stdout in a SoR format.
 */
class PrintFielder : public Fielder {
public:
  Row *row_;
  bool completed_;

  /**
   * Constructs a PrintFielder.
   * @param row  The row that this fielder will be iterating through. The row
   *        passed remains external.
   */
  explicit PrintFielder(Row& row) {
    this->row_ = &row;
    this->completed_ = false;
  }

  void start(size_t r) override {
    // Visit each column within the row provided to the fielder.
    for (size_t c = 0; c < this->row_->width(); c++) {
      this->row_->visit(c, *this);
    }
  }

  void accept(bool b) override {
    printf("<%d> ", b);
  }

  void accept(float f) override {
    printf("<%f> ", f);
  }

  void accept(int i) override {
    printf("<%d> ", i);
  }

  void accept(String* s) override {
    if (s != nullptr) {
      printf("<%s> ", s->c_str());
    }
    else {
      printf("<> ");
    }
  }

  void done() override {
    this->completed_ = true;
  }
};

/**
 * An implementation of a Rower that goes through a dataframe and prints the
 * values in a SoR format to stdout.
 */
class PrintRower : public Rower {
public:
  bool accept(Row &r) override {
    // Use a fielder to print out this row.
    PrintFielder * fielder = new PrintFielder(r);
    fielder->start(r.get_idx());
    fielder->done();

    // Go to the next line in the stdout.
    printf("\n");
    return true;
  }

  void join_delete(Rower *other) override {
    // Do nothing because there is nothing to join
  }
};

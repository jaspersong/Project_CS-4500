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
 * An implementation of a Fielder that accepts the values of the row with a
 * schema comprised entirely of integer and float column data types, stores
 * the summation of all of the values of that row. To get the summation once the
 * row has been iterated through, call get_summation(). If any of fields
 * within the row is not integer or float, the fielder will throw an error to
 * stdout and terminate the program.
 */
class SummationFielder : public Fielder {
public:
  double summation_;
  Row * row_;
  bool completed_;

  /**
   * Constructs a fielder that will visit each of the fields within the
   * provided column upon calling start().
   * @param row The row that this fielder will be iterating through. The row
   *            passed remains external.
   */
  SummationFielder(Row& row) {
    this->row_ = &row;
    this->summation_ = 0.0;
    this->completed_ = false;
  }

  void start(size_t r) {
    // Visit each column within the row provided to the fielder.
    for (size_t c = 0; c < this->row_->width(); c++) {
      this->row_->visit(c, *this);
    }
  }

  void accept(bool b) {
    printf("This fielder does not accept bool types.\n");
    exit(1);
  }

  void accept(float f) {
    this->summation_ += static_cast<double>(f);
  }

  void accept(int i) {
    this->summation_ += static_cast<double>(i);
  }

  void accept(String* s) {
    printf("This fielder does not accept string types.\n");
    exit(1);
  }

  void done() {
    this->completed_ = true;
  }

  /**
   * Retrieves the summation of the values within the row that the fielder is
   * visiting. If the start() function was not called, it will return with 0.
   * If the fielder is not done iterating through the fields within the
   * column, it will also return 0.
   * @return The summation of all of the values that had been seen by this
   *         fielder.
   */
  double get_summation() {
    if (this->completed_) {
      return this->summation_;
    }
    else {
      return 0.0;
    }
  }
};

/**
 * An implementation of Rower that goes through a dataframe comprised
 * entirely of floats and integer type columns and determines if the
 * summation of the values in that row is greater than a specified value.
 * In addition, it sums up all of the values that had been accepted into this
 * rower. The current summation can be retrieved by get_summation().
 */
class SummationGreaterThanRower : public Rower {
public:
  double threshold_;
  double summation_;

  /**
   * Constructs a SummationGreaterThanRower.
   * @param threshold The threshold that the row summation should be greater
   *        than in order for the accept() function to be true.
   */
  SummationGreaterThanRower(double threshold) {
    this->threshold_ = threshold;
    this->summation_ = 0.0;
  }

  /**
   * Constructs a SummationGreaterThanRower.
   * @param threshold The threshold that the row summation should be greater
   *        than in order for the accept() function to be true.
   */
  SummationGreaterThanRower(int threshold)
      : SummationGreaterThanRower(static_cast<double>(threshold)) {}

  /**
   * Constructs a SummationGreaterThanRower.
   * @param threshold The threshold that the row summation should be greater
   *        than in order for the accept() function to be true.
   */
  SummationGreaterThanRower(float threshold)
      : SummationGreaterThanRower(static_cast<double>(threshold)) {}

  bool accept(Row &r) {
    // Sum up all the values within the row by using the summation fielder
    SummationFielder * fielder = new SummationFielder(r);
    fielder->start(r.get_idx());
    fielder->done();

    // Now get the summation
    double row_summation = fielder->get_summation();
    this->summation_ += row_summation;

    // Free the memory for the fielder, and then return whether or not this
    // row summation is over the threshold
    delete fielder;
    return (row_summation > this->threshold_);
  }

  void join_delete(Rower *other) {
    // Cast the other rower to the same class. If it is not the same class,
    // then the rower was not used properly.
    SummationGreaterThanRower *o = dynamic_cast<SummationGreaterThanRower *>
            (other);

    // Add the summation from the other rower to this one
    this->summation_ += o->get_summation();

    // Free the memory
    delete other;
  }

  /**
   * Retrieves the current summation of all of the values that the Rower has
   * seen thus far. If it has not seen any values from a dataframe yet, then
   * it will return with 0.
   * @return The summation of all of the values that the Rower has seen thus
   *        far.
   */
  double get_summation() {
    return this->summation_;
  }

  Object* clone() {
    return new SummationGreaterThanRower(this->threshold_);
  }
};

/**
 * Name: Snowy Chen, Joe Song
 * Date: 21 February 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// lang::CwC

#pragma once

#include <math.h>
#include <stdlib.h>

#include "object.h"

#include "fielder.h"
#include "row.h"
#include "rower.h"

/**
 * An implementation thst accepts the values of the row with schema
 * comprised entirely of integer and float column data types, and sums up all
 * of the squares of the fields. To get the summation once the row has been
 * iterated through, call get_squared_summation(). If any of the fields
 * within the row is not an integer or float, the fielder will throw an error
 * to stdout and terminate the program.
 */
class SquaredSummationFielder : public Fielder {
public:
  double squared_summation_;
  Row *row_;
  bool completed_;

  /**
   * Constructs a fielder that will visit each of the fields within the
   * provided column upon calling start().
   * @param row The row that this fielder will be iterating through. The
   *            fielder does not own the passed in row.
   */
  SquaredSummationFielder(Row &row) {
    this->row_ = &row;
    this->squared_summation_ = 0.0;
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
    this->squared_summation_ += pow(static_cast<double>(f), 2.0);
  }

  void accept(int i) {
    this->squared_summation_ += pow(static_cast<double>(i), 2.0);
  }

  void accept(String *s) {
    printf("This fielder does not accept string types.\n");
    exit(1);
  }

  void done() { this->completed_ = true; }

  /**
   * Retrieves the summation of the squared values within the row that the
   * fielder had been visiting. If the start() function was not called, it
   * will return with 0. If the fielder is not done iterating through the
   * fields within the column it will also return 0.
   * @return The summation of all the squared values that had been seen by
   *         this fielder.
   */
  double get_squared_summation() {
    if (this->completed_) {
      return this->squared_summation_;
    } else {
      return 0.0;
    }
  }
};

/**
 * An implementation of Rower that goes through a dataframe comprised
 * entirely of floats and integer type columns. It treats each row in a
 * dataframe as a vector, and then calculates the Euclidean normal magnitude
 * of each of the vectors. The calculated Euclidean magnitude can be queried
 * by get_euclidean_magnitude(), and the average Euclidean magnitude can be
 * queried by get_average().
 *
 * NOTE: get_euclidean_magnitude() and get_average() are NOT thread-safe. If
 * the rower did not, or have not yet, calculated the Euclidean magnitude of
 * a particular row, it will return with 0. The average will output of the
 * average of the vectors that it had calculated thus far.
 */
class EuclideanMagnitudeRower : public Rower {
public:
  size_t num_vectors_;
  double *magnitudes_;
  bool *has_seen_vectors_;

  double summation_magnitude_;
  size_t num_vectors_seen_;

  /**
   * Constructs a EuclideanMagnitudeRower.
   * @param num_of_vectors The number of vectors within the dataframe. If the
   *        rower is being used by a dataframe with a different number of
   *        rows, the behavior is undefined.
   */
  EuclideanMagnitudeRower(size_t num_of_vectors) {
    this->num_vectors_ = num_of_vectors;
    this->magnitudes_ = new double[num_of_vectors];
    this->has_seen_vectors_ = new bool[num_of_vectors];

    // Initialize the arrays.
    for (size_t i = 0; i < this->num_vectors_; i++) {
      this->magnitudes_[i] = 0.0;
      this->has_seen_vectors_[i] = false;
    }

    this->summation_magnitude_ = 0.0;
    this->num_vectors_seen_ = 0;
  }

  /**
   * Deconstructs the rower.
   */
  ~EuclideanMagnitudeRower() {
    delete[] this->magnitudes_;
    delete[] this->has_seen_vectors_;
  }

  bool accept(Row &r) {
    // Sum up all the squared values within this row.
    SquaredSummationFielder *fielder = new SquaredSummationFielder(r);
    fielder->start(r.get_idx());
    fielder->done();

    // Now calculate the Euclidean magnitude
    double magnitude = sqrt(fielder->get_squared_summation());

    // Save the value
    this->magnitudes_[r.get_idx()] = magnitude;
    this->has_seen_vectors_[r.get_idx()] = true;
    this->summation_magnitude_ += magnitude;
    this->num_vectors_seen_ += 1;

    // Free the memory for the fielder, and then return whether or not this
    // row summation is over the threshold
    delete fielder;

    // Return a default true.
    return true;
  }

  void join_delete(Rower *other) {
    // Cast the other rower to the same class. If it is not the same class,
    // then the rower was not used properly.
    EuclideanMagnitudeRower *o = dynamic_cast<EuclideanMagnitudeRower *>(other);

    // Iterate through the magnitudes stored within the other rower and put
    // the information into this rower
    this->summation_magnitude_ += o->summation_magnitude_;
    this->num_vectors_seen_ += o->num_vectors_seen_;
    for (size_t i = 0; i < this->num_vectors_; i++) {
      if (o->has_seen_vectors_[i]) {
        this->magnitudes_[i] = o->magnitudes_[i];
        this->has_seen_vectors_[i] = true;
      }
    }

    // Free the memory
    delete other;
  }

  /**
   * Retrieves the calculated Euclidean magnitude of the vector at the given
   * 0-indexed row index.
   *
   * NOTE: This function is not thread-safe. If this function is called
   * before the rower is able to calculate the magnitude of the provided row,
   * it will automatically return with 0.0.
   * @param idx The 0-indexed row index.
   * @return The Euclidean magnitude of the vector at the specified index.
   * @throws If the provided row index is out of bounds, the function will
   *         print an error message to stdout and terminate the program.
   */
  double get_euclidean_magnitude(size_t idx) {
    if ((idx < 0) || (idx > this->num_vectors_)) {
      printf("%zu is an invalid row index.\n", idx);
      exit(1);
    }
    else if (this->has_seen_vectors_[idx]) {
      return this->magnitudes_[idx];
    }
    else {
      return 0.0;
    }
  }

  /**
   * Retrieves the average magnitude of all of the vectors it has seen thus far.
   * @return The average magnitude. If it has not seen any vectors, it will
   *         return with 0.0.
   */
  double get_average() {
    if (this->num_vectors_seen_ <= 0) {
      return 0.0;
    }
    else {
      return this->summation_magnitude_ / this->num_vectors_seen_;
    }
  }

  /**
   * Gets the number of vectors that this rower is storing.
   * @return The number of vectors this rower can store.
   */
  size_t get_num_vectors() {
    return this->num_vectors_;
  }

  Object *clone() { return new EuclideanMagnitudeRower(this->num_vectors_); }
};

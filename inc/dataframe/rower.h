/**
 * Name: Snowy Chen, Joe Song
 * Date: 14 February 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// lang::CwC

#pragma once

#include "custom_object.h"
#include "row.h"

/*******************************************************************************
 *  Rower::
 *  An interface for iterating through each row of a data frame. The intent
 *  is that this class should subclassed and the accept() method be given
 *  a meaningful implementation. Rowers can be cloned for parallel execution.
 */
class Rower : public CustomObject {
public:
  /** This method is called once per row. The row object is on loan and
      should not be retained as it is likely going to be reused in the next
      call. The return value is used in filters to indicate that a row
      should be kept. */
  virtual bool accept(Row &r) { return false; }
};

/**
 * A rower that goes down the rows and populates the row to the particular
 * schema.
 */
class Writer : public CustomObject {
public:
  /**
   * This method is called once per row. The row object will be used to
   * populate a dataframe. It should not be retained, as it can be reused in
   * the next call.
   * @param r The row to populate
   */
  virtual void visit(Row& r) {}

  /**
   * Returns whether or not there is any more data to write into a dataframe.
   * @return True if there is no more data. False if otherwise.
   */
  virtual bool done() { return true; }
};

/**
 * A rower that goes down the rows of a pre-existing dataframe and reads the
 * data.
 */
class Reader : public Rower {};

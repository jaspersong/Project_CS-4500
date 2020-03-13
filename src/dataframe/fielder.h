/**
 * Name: Snowy Chen, Joe Song
 * Date: 14 February 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

//lang::CwC

#pragma once

#include <stdlib.h>

#include "custom_object.h"
#include "custom_string.h"

/*****************************************************************************
 * Fielder::
 * A field visitor invoked by Row.
 */
class Fielder : public CustomObject {
public:
  /** Called before visiting a row, the argument is the row offset in the
    dataframe. */
  virtual void start(size_t r) {}

  /** Called for fields of the argument's type with the value of the field. */
  virtual void accept(bool b) {}

  virtual void accept(float f) {}

  virtual void accept(int i) {}

  virtual void accept(String *s) {}

  /** Called when all fields have been seen. */
  virtual void done() {}
};

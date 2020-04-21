/**
 * Name: Snowy Chen, Joe Song
 * Date: 24 March 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// lang::Cpp

#pragma once

#include "custom_object.h"
#include "custom_string.h"

#include "dataframe.h"
#include "row.h"
#include "rower.h"

/**
 * An implementation of a Rower that goes through a dataframe and copies it
 * into the end of another dataframe with the exact same schema. Copying
 * dataframes with differing schemas will result in undefined behavior.
 */
class CopyWriter : public Writer {
public:
  explicit CopyWriter(DataFrame *source_dataframe);
  void visit(Row &r) override;
  bool done() override;

private:
  DataFrame *source_dataframe;
  size_t curr_row;
};

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

#include "fielder.h"
#include "row.h"
#include "rower.h"
#include "dataframe.h"

/**
 * An implementation of a Rower that goes through a dataframe and copies it
 * into the end of another dataframe with the exact same schema. Copying
 * dataframes with differing schemas will result in undefined behavior.
 */
class CopyRower : public Rower {
public:
  explicit CopyRower(DataFrame *dest_dataframe);
  bool accept(Row &r) override;

private:
  DataFrame *dest_dataframe;
};

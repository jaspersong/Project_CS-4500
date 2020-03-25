/**
 * Name: Snowy Chen, Joe Song
 * Date: 24 March 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::Cpp

#include "copy_rower.h"

CopyRower::CopyRower(DataFrame *dest_dataframe) {
  assert(dest_dataframe != nullptr);
  this->dest_dataframe = dest_dataframe;
}

bool CopyRower::accept(Row &r) {
  this->dest_dataframe->add_row(r);
  return true;
}

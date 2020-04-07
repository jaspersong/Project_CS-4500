/**
 * Name: Snowy Chen, Joe Song
 * Date: 24 March 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::Cpp

#include "copy_writer.h"

CopyWriter::CopyWriter(DataFrame *source_dataframe) {
  assert(source_dataframe != nullptr);
  this->source_dataframe = source_dataframe;
  this->curr_row = 0;
}

void CopyWriter::visit(Row &r) {
  // Go through the columns and copy the data into the row
  for (size_t c = 0; c < this->source_dataframe->ncols(); c++) {
    // Handle adding to the rows based off of the type of the column
    switch (this->source_dataframe->get_schema().col_type(c)) {
    case ColumnType_Bool: {
      r.set(c, this->source_dataframe->get_bool(c, this->curr_row));
      break;
    }
    case ColumnType_Integer: {
      r.set(c, this->source_dataframe->get_int(c, this->curr_row));
      break;
    }
    case ColumnType_Float: {
      r.set(c, this->source_dataframe->get_float(c, this->curr_row));
      break;
    }
    case ColumnType_String:
    default: {
      r.set(c, this->source_dataframe->get_string(c, this->curr_row));
    }
    }
  }

  this->curr_row += 1;
}

bool CopyWriter::done() {
  return this->curr_row >= this->source_dataframe->nrows();
}

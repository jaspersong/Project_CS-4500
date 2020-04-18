/**
 * Name: Snowy Chen, Joe Song
 * Date: 21 March 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::Cpp

#include "schema.h"
#include "dataframe_column.h"

Schema::Schema(Schema &from) : Schema() {
  // Copy the information about the columns into this schema's column list
  this->column_list.resize(from.width());
  for (size_t i = 0; i < from.width(); i++) {
    this->column_list[i] = from.col_type(i);
  }
}

Schema::Schema() = default;

Schema::Schema(const char *types) : Schema() {
  assert(types);

  // Iterate through the characters within the types until we reach the
  // null terminator.
  for (int i = 0; types[i] != '\0'; i++) {
    this->add_column(types[i]);
  }
}

void Schema::add_column(char typ) {
  // Get the desired column type and create a new column to add to the
  // list
  assert((typ == static_cast<char>(ColumnType_Bool)) ||
         (typ == static_cast<char>(ColumnType_Integer)) ||
         (typ == static_cast<char>(ColumnType_Float)) ||
         (typ == static_cast<char>(ColumnType_String)));

  this->column_list.push_back(static_cast<ColumnType_t>(typ));
}

ColumnType_t Schema::col_type(size_t idx) {
  return this->column_list[idx];
}

size_t Schema::width() { return this->column_list.size(); }

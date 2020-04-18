/**
 * Name: Snowy Chen, Joe Song
 * Date: 21 March 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::Cpp

#include "row.h"
#include "dataframe_column.h"

/** Build a row following a schema. */
Row::Row(Schema &scm) : schema(scm) {
  DataItem dummy;
  dummy.s = nullptr;
  this->value_list.resize(this->schema.width(), dummy);
}

Row::~Row() {
  for (size_t i = 0; i < this->width(); i++) {
    if (this->schema.col_type(i) == ColumnType_String) {
      delete this->get_string(i);
    }
  }
}

void Row::set(size_t col, int val) {
  // Verify that this column index takes in an integer
  assert(this->schema.col_type(col) == static_cast<char>(ColumnType_Integer));
  DataItem value;
  value.i = val;
  this->value_list[col] = value;
}

void Row::set(size_t col, float val) {
  // Verify that this column index takes in a float
  assert(this->schema.col_type(col) == static_cast<char>(ColumnType_Float));
  DataItem value;
  value.f = val;
  this->value_list[col] = value;
}

void Row::set(size_t col, bool val) {
  // Verify that this column index takes in a bool
  assert(this->schema.col_type(col) == static_cast<char>(ColumnType_Bool));
  DataItem value;
  value.b = val;
  this->value_list[col] = value;
}

void Row::set(size_t col, String *val) {
  // Verify that this column index takes in a string
  assert(this->schema.col_type(col) == static_cast<char>(ColumnType_String));

  // Delete the old one
  DataItem value = this->value_list.at(col);
  delete value.s;

  // Add the new item
  if (val != nullptr) {
    value.s = new String(*val);
  } else {
    value.s = nullptr;
  }
  this->value_list[col] = value;
}

int Row::get_int(size_t col) {
  // Verify that this column index takes in an integer
  assert(this->schema.col_type(col) == static_cast<char>(ColumnType_Integer));

  DataItem item = this->value_list.at(col);
  return item.i;
}

bool Row::get_bool(size_t col) {
  // Verify that this column index takes in a bool
  assert(this->schema.col_type(col) == static_cast<char>(ColumnType_Bool));

  DataItem item = this->value_list.at(col);
  return item.b;
}

float Row::get_float(size_t col) {
  // Verify that this column index takes in a float
  assert(this->schema.col_type(col) == static_cast<char>(ColumnType_Float));

  DataItem item = this->value_list.at(col);
  return item.f;
}

String *Row::get_string(size_t col) {
  assert(this->schema.col_type(col) == static_cast<char>(ColumnType_String));

  DataItem item = this->value_list.at(col);
  return item.s;
}

size_t Row::width() {
  // Assuming number of fields is the number of columns for this row, and
  // does not necessarily mean the count of how many fields have been filled.
  return this->schema.width();
}

char Row::col_type(size_t idx) { return this->schema.col_type(idx); }

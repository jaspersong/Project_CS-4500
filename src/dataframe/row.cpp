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
Row::Row(Schema &scm) {
  this->schema = &scm;
  this->value_list = new ArrayOfArrays();

  // Default the row index to 0 for now.
  this->row_index = 0;

  // Initialize the values to all 0s
  while (this->value_list->size() < this->schema->width()) {
    // Fill the value list until it has the same number of columns as the
    // schema
    DataItem_ data_item;
    data_item.o = nullptr;
    while (this->value_list->size() < this->schema->width()) {
      this->value_list->add_new_item(data_item);
    }
  }
}

Row::~Row() { delete this->value_list; }

void Row::set(size_t col, int val) {
  // Verify that this column index takes in an integer
  if (this->schema->col_type(col) != static_cast<char>(ColumnType_Integer)) {
    printf("Attempted to insert the wrong data type to column.\n");
    exit(1);
  }

  DataItem_ value;
  value.i = val;
  this->value_list->set_new_item(col, value);
}

void Row::set(size_t col, float val) {
  // Verify that this column index takes in a float
  if (this->schema->col_type(col) != static_cast<char>(ColumnType_Float)) {
    printf("Attempted to insert the wrong data type to column.\n");
    exit(1);
  }

  DataItem_ value;
  value.f = val;
  this->value_list->set_new_item(col, value);
}

void Row::set(size_t col, bool val) {
  // Verify that this column index takes in a bool
  if (this->schema->col_type(col) != static_cast<char>(ColumnType_Bool)) {
    printf("Attempted to insert the wrong data type to column.\n");
    exit(1);
  }

  DataItem_ value;
  value.b = val;
  this->value_list->set_new_item(col, value);
}

void Row::set(size_t col, String *val) {
  // Verify that this column index takes in a string
  if (this->schema->col_type(col) != static_cast<char>(ColumnType_String)) {
    printf("Attempted to insert the wrong data type to column.\n");
    exit(1);
  }

  DataItem_ value;
  value.s = val;
  this->value_list->set_new_item(col, value);
}

void Row::set_idx(size_t idx) { this->row_index = idx; }

size_t Row::get_idx() { return this->row_index; }

int Row::get_int(size_t col) {
  // Verify that this column index takes in an integer
  if (this->schema->col_type(col) != static_cast<char>(ColumnType_Integer)) {
    printf("Attempted to get an integer from a non-integer column.\n");
    exit(1);
  }

  DataItem_ item = this->value_list->get_item(col);
  return item.i;
}

bool Row::get_bool(size_t col) {
  // Verify that this column index takes in a bool
  if (this->schema->col_type(col) != static_cast<char>(ColumnType_Bool)) {
    printf("Attempted to get a bool from a non-bool column.\n");
    exit(1);
  }

  DataItem_ item = this->value_list->get_item(col);
  return item.b;
}

float Row::get_float(size_t col) {
  // Verify that this column index takes in a float
  if (this->schema->col_type(col) != static_cast<char>(ColumnType_Float)) {
    printf("Attempted to get a float from a non-float column.\n");
    exit(1);
  }

  DataItem_ item = this->value_list->get_item(col);
  return item.f;
}

String *Row::get_string(size_t col) {
  // Verify that this column index takes in a string
  if (this->schema->col_type(col) != static_cast<char>(ColumnType_String)) {
    printf("Attempted to get a string from a non-string column.\n");
    exit(1);
  }

  DataItem_ item = this->value_list->get_item(col);
  return item.s;
}

size_t Row::width() {
  // Assuming number of fields is the number of columns for this row, and
  // does not necessarily mean the count of how many fields have been filled.
  return this->schema->width();
}

char Row::col_type(size_t idx) { return this->schema->col_type(idx); }

void Row::visit(size_t idx, Fielder &f) {
  // Get the item and then pass it into the fielder
  DataItem_ item = this->value_list->get_item(idx);

  // Now pass it to the fielder according to the column type
  switch (this->schema->col_type(idx)) {
  case ColumnType_Bool:
    f.accept(item.b);
    break;
  case ColumnType_Integer:
    f.accept(item.i);
    break;
  case ColumnType_Float:
    f.accept(item.f);
    break;
  case ColumnType_String:
  default:
    f.accept(item.s);
    break;
  }
}

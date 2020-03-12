/**
 * Name: Snowy Chen, Joe Song
 * Date: 14 February 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

//lang::CwC

#pragma once

#include <stdlib.h>

#include "object.h"
#include "custom_string.h"

#include "fielder.h"
#include "schema.h"

/*************************************************************************
 * Row::
 *
 * This class represents a single row of data constructed according to a
 * dataframe's schema. The purpose of this class is to make it easier to add
 * read/write complete rows. Internally a dataframe hold data in columns.
 * Rows have pointer equality.
 */
class Row : public Object {
public:
  Schema *schema_;
  LinkedListArray_ *value_list;
  size_t row_index;

  /** Build a row following a schema. */
  explicit Row(Schema &scm) {
    this->schema_ = &scm;
    this->value_list = new LinkedListArray_();

    // Default the row index to 0 for now.
    this->row_index = 0;

    // Initialize the values to all 0s
    while (this->value_list->size() < this->schema_->width()) {
      // Fill the value list until it has the same number of columns as the schema
      DataItem_ data_item;
      data_item.o = nullptr;
      while (this->value_list->size() < this->schema_->width()) {
        this->value_list->add_new_item(data_item);
      }
    }
  }

  /**
   * Deconstructs the row.
   */
  ~Row() override { delete this->value_list; }

  /** Setters: set the given column with the given value. Setting a column with
   * a value of the wrong type is undefined. */
  void set(size_t col, int val) {
    // Verify that this column index takes in an integer
    if (this->schema_->col_type(col) != static_cast<char>(ColumnType_Integer)) {
      printf("Attempted to insert the wrong data type to column.\n");
      exit(1);
    }

    DataItem_ value;
    value.i = val;
    this->value_list->set_new_item(col, value);
  }

  void set(size_t col, float val) {
    // Verify that this column index takes in a float
    if (this->schema_->col_type(col) != static_cast<char>(ColumnType_Float)) {
      printf("Attempted to insert the wrong data type to column.\n");
      exit(1);
    }

    DataItem_ value;
    value.f = val;
    this->value_list->set_new_item(col, value);
  }

  void set(size_t col, bool val) {
    // Verify that this column index takes in a bool
    if (this->schema_->col_type(col) != static_cast<char>(ColumnType_Bool)) {
      printf("Attempted to insert the wrong data type to column.\n");
      exit(1);
    }

    DataItem_ value;
    value.b = val;
    this->value_list->set_new_item(col, value);
  }

  /** The string is external. */
  void set(size_t col, String *val) {
    // Verify that this column index takes in a string
    if (this->schema_->col_type(col) != static_cast<char>(ColumnType_String)) {
      printf("Attempted to insert the wrong data type to column.\n");
      exit(1);
    }

    DataItem_ value;
    value.s = val;
    this->value_list->set_new_item(col, value);
  }

  /** Set/get the index of this row (ie. its position in the dataframe. This is
   *  only used for informational purposes, unused otherwise */
  void set_idx(size_t idx) {
    this->row_index = idx;
  }

  size_t get_idx() {
    return this->row_index;
  }

  /** Getters: get the value at the given column. If the column is not
   * of the requested type, the result is undefined. */
  int get_int(size_t col) {
    // Verify that this column index takes in an integer
    if (this->schema_->col_type(col) != static_cast<char>(ColumnType_Integer)) {
      printf("Attempted to get an integer from a non-integer column.\n");
      exit(1);
    }

    DataItem_ item = this->value_list->get_item(col);
    return item.i;
  }

  bool get_bool(size_t col) {
    // Verify that this column index takes in a bool
    if (this->schema_->col_type(col) != static_cast<char>(ColumnType_Bool)) {
      printf("Attempted to get a bool from a non-bool column.\n");
      exit(1);
    }

    DataItem_ item = this->value_list->get_item(col);
    return item.b;
  }

  float get_float(size_t col) {
    // Verify that this column index takes in a float
    if (this->schema_->col_type(col) != static_cast<char>(ColumnType_Float)) {
      printf("Attempted to get a float from a non-float column.\n");
      exit(1);
    }

    DataItem_ item = this->value_list->get_item(col);
    return item.f;
  }

  String *get_string(size_t col) {
    // Verify that this column index takes in a string
    if (this->schema_->col_type(col) != static_cast<char>(ColumnType_String)) {
      printf("Attempted to get a string from a non-string column.\n");
      exit(1);
    }

    DataItem_ item = this->value_list->get_item(col);
    return item.s;
  }

  /** Number of fields in the row. */
  size_t width() {
    // Assuming number of fields is the number of columns for this row, and
    // does not necessarily mean the count of how many fields have been filled.
    return this->schema_->width();
  }

  /** Type of the field at the given position. An idx >= width is  undefined. */
  char col_type(size_t idx) {
    return this->schema_->col_type(idx);
  }

  /** Given a Fielder, visit every field of this row.
   * Calling this method before the row's fields have been set is undefined. */
  void visit(size_t idx, Fielder &f) {
    // Get the item and then pass it into the fielder
    DataItem_ item = this->value_list->get_item(idx);

    // Now pass it to the fielder according to the column type
    switch (this->schema_->col_type(idx)) {
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
};

/**
 * Name: Snowy Chen, Joe Song
 * Date: 14 February 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// lang::CwC

#pragma once

#include "custom_object.h"
#include "custom_string.h"

#include "dataframe_helper.h"
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
class Row : public CustomObject {
public:
  /** Build a row following a schema. */
  explicit Row(Schema &scm);
  ~Row() override;

  /** Setters: set the given column with the given value. Setting a column with
   * a value of the wrong type is undefined. */
  void set(size_t col, int val);
  void set(size_t col, float val);
  void set(size_t col, bool val);
  /** The string is external. */
  void set(size_t col, String *val);

  /** Set/get the index of this row (ie. its position in the dataframe. This is
   *  only used for informational purposes, unused otherwise */
  void set_idx(size_t idx);
  size_t get_idx();

  /** Getters: get the value at the given column. If the column is not
   * of the requested type, the result is undefined. */
  int get_int(size_t col);
  bool get_bool(size_t col);
  float get_float(size_t col);
  String *get_string(size_t col);

  /** Number of fields in the row. */
  size_t width();

  /** Type of the field at the given position. An idx >= width is  undefined. */
  char col_type(size_t idx);

  /** Given a Fielder, visit every field of this row.
   * Calling this method before the row's fields have been set is undefined. */
  void visit(size_t idx, Fielder &f);

private:
  Schema *schema;
  ArrayOfArrays *value_list;
  size_t row_index;
};

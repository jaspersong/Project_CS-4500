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
#include "dataframe_column.h"
#include "dataframe_helper.h"
#include "row.h"
#include "rower.h"
#include "schema.h"

/****************************************************************************
 * DataFrame::
 *
 * A DataFrame is table composed of columns of equal length. Each column
 * holds values of the same type (I, S, B, F). A dataframe has a schema that
 * describes it.
 */
class DataFrame : public CustomObject {
public:
  /** Create a data frame with the same columns as the give df but no rows */
  DataFrame(DataFrame &df);

  /** Create a data frame from a schema and columns. All columns are created
   * empty. */
  explicit DataFrame(Schema &schema);

  /**
   * Deconstructs the dataframe
   */
  ~DataFrame() override;

  /** Returns the dataframe's schema. Modifying the schema after a dataframe
   * has been created in undefined. */
  Schema &get_schema();

  /** Adds a column this dataframe, updates the schema, the new column
   * is external, and appears as the last column of the dataframe, the
   * name is optional and external. A nullptr colum is undefined. */
  void add_column(DF_Column *col);

  /** Return the value at the given column and row. Accessing rows or
   *  columns out of bounds, or request the wrong type is undefined.*/
  int get_int(size_t col, size_t row);
  bool get_bool(size_t col, size_t row);
  float get_float(size_t col, size_t row);
  String *get_string(size_t col, size_t row);

  /** Set the value at the given column and row to the given value.
   * If the column is not  of the right type or the indices are out of
   * bound, the result is undefined. */
  void set(size_t col, size_t row, int val);
  void set(size_t col, size_t row, bool val);
  void set(size_t col, size_t row, float val);
  void set(size_t col, size_t row, String *val);

  /** Set the fields of the given row object with values from the columns at
   * the given offset.  If the row is not from the same schema as the
   * dataframe, results are undefined.
   */
  void fill_row(size_t idx, Row &row);

  /** Add a row at the end of this dataframe. The row is expected to have
   *  the right schema and be filled with values, otherwise undedined.  */
  void add_row(Row &row);

  /** The number of rows in the dataframe. */
  size_t nrows();

  /** The number of columns in the dataframe.*/
  size_t ncols();

  /** Visit rows in order */
  void map(Rower &r);

  /** Print the dataframe in SoR format to standard output. */
  void print();

private:
  Schema *schema;

  // ASSERT: This column list will always have the same data type as the
  // schema at any given 0-indexed column index within the bounds of the
  // schema width.
  ArrayOfArrays *col_list;

  // Cache of the number of rows
  size_t num_rows;

  /**
   * A helper function that takes in a column, row and column type and
   * verifies that the given coordinate is valid and can execute functions
   * related to the column data type provided.
   * @param col 0-indexed column index.
   * @param row 0-indexed row index.
   * @param type The expected column data type.
   * @return True if everything is invalid. False if otherwise. If it is
   *         false, it will also print out the error message to stdout.
   */
  bool verify_col_row_parameters(size_t col, size_t row, ColumnType_t type);

  /**
   * Helper function that creates a row of the contents at the specified
   * 0-indexed row index. This function should only be called by DataFrame
   * and RowerThread_.
   * @param row 0-indexed row index.
   * @return A dynamically allocated row of the contents at the specified
   *         index. The caller will own the return value.
   * @throws Prints an error message and terminates the program if the
   *         specified index is invalid
   */
  Row *create_row(size_t row);
};

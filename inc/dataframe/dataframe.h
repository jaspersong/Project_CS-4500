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
#include "row.h"
#include "rower.h"
#include "schema.h"
#include <vector>

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

  void serialize(Serializer &serializer) override;
  size_t serialization_required_bytes() override;
  static DataFrame *deserialize_as_dataframe(Deserializer &deserializer);

private:
  Schema schema;

  // ASSERT: This column list will always have the same data type as the
  // schema at any given 0-indexed column index within the bounds of the
  // schema width.
  std::vector<DF_Column *> col_list;

  // Cache of the number of rows
  size_t num_rows;

  /**
   * Helper function that copy the row of the contents at the specified
   * 0-indexed row index.
   * @param row 0-indexed row index.
   * @return A dynamically allocated row of the contents at the specified
   *         index. The caller will own the return value.
   * @throws Prints an error message and terminates the program if the
   *         specified index is invalid
   */
  Row *copy_row(size_t row);
};

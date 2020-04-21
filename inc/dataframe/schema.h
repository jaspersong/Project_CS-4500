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
#include <vector>

/*************************************************************************
 * Schema::
 * A schema is a description of the contents of a data frame, the schema
 * knows the number of columns and number of rows, the type of each column,
 * optionally columns and rows can be named by strings.
 * The valid types are represented by the chars 'S', 'B', 'I' and 'F'.
 */
class Schema : public CustomObject {
public:
  /** Copying constructor */
  Schema(Schema &from);

  /** Create an empty schema **/
  Schema();

  /** Create a schema from a string of types. A string that contains
   * characters other than those identifying the four type results in
   * undefined behavior. The argument is external, a nullptr argument is
   * undefined. **/
  explicit Schema(const char *types);

  /**
   * Adds a column of the given type to the schema.
   * @param typ
   */
  void add_column(char typ);

  /** Return type of column at idx. An idx >= width is undefined. */
  ColumnType_t col_type(size_t idx);

  /** The number of columns */
  size_t width();

private:
  std::vector<ColumnType_t> column_list;
};

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

/**
 * Class prototypes
 */
class DF_IntColumn;
class DF_BoolColumn;
class DF_FloatColumn;
class DF_StringColumn;

/**
 * An enumeration that contains the column type represented in a char. The
 * purpose of this is to easily define which character is which data type to
 * increase code readability, so that when the enumeration gets casted into a
 * char, it outputs the appropriate character to the specifications.
 */
typedef enum {
  ColumnType_Bool = 'B',
  ColumnType_Integer = 'I',
  ColumnType_Float = 'F',
  ColumnType_String = 'S',
} ColumnType_t;

/**************************************************************************
 * Column ::
 * Represents one column of a data frame which holds values of a single type.
 * This abstract class defines methods overriden in subclasses. There is
 * one subclass per element type. Columns are mutable, equality is pointer
 * equality. */
class DF_Column : public CustomObject {
public:
  // Constructs a column
  explicit DF_Column(ColumnType_t column_type);

  // Deconstructs the column
  ~DF_Column() override;

  /** Type converters: Return same column under its actual type, or
   *  nullptr if of the wrong type.  */
  virtual DF_IntColumn *as_int() { return nullptr; }
  virtual DF_BoolColumn *as_bool() { return nullptr; }
  virtual DF_FloatColumn *as_float() { return nullptr; }
  virtual DF_StringColumn *as_string() { return nullptr; }

  /** Type appropriate push_back methods. Calling the wrong method is
   * undefined behavior. **/
  void push_back(int val);
  void push_back(bool val);
  void push_back(float val);
  void push_back(String *val);

  /** Returns the number of elements in the column. */
  size_t size();

  /** Return the type of this column as a char: 'S', 'B', 'I' and 'F'.**/
  char get_type();

protected:
  // A protected variable containing all of the data within the column. It
  // can be used by its derivative classes.
  ArrayOfArrays *list;

  // The data type of the column. All inherited columns should set this value
  // to the appropriate column data type upon construction.
  ColumnType_t data_type;
};

/*************************************************************************
 * BoolColumn::
 * Holds primitive int values, unwrapped.
 */
class DF_BoolColumn : public DF_Column {
public:
  DF_BoolColumn();

  // Constructs a column by copying the data from the provided column into
  // this new instance of column.
  DF_BoolColumn(DF_BoolColumn &column);

  bool get(size_t idx);

  DF_BoolColumn *as_bool() override { return this; }

  void set(size_t idx, bool val);
};

/*************************************************************************
 * IntColumn::
 * Holds int values.
 */
class DF_IntColumn : public DF_Column {
public:
  DF_IntColumn();

  // Constructs a column by copying the data from the provided column into
  // this new instance of column.
  DF_IntColumn(DF_IntColumn &column);

  int get(size_t idx);

  DF_IntColumn *as_int() override { return this; }

  /** Set value at idx. An out of bound idx is undefined.  */
  void set(size_t idx, int val);
};

/*************************************************************************
 * FloatColumn::
 * Holds primitive int values, unwrapped.
 */
class DF_FloatColumn : public DF_Column {
public:
  DF_FloatColumn();

  // Constructs a column by copying the data from the provided column into
  // this new instance of column.
  DF_FloatColumn(DF_FloatColumn &column);

  float get(size_t idx);

  DF_FloatColumn *as_float() override { return this; }

  void set(size_t idx, float val);
};

/*************************************************************************
 * StringColumn::
 * Holds string pointers. The strings are external.  Nullptr is a valid
 * value.
 */
class DF_StringColumn : public DF_Column {
public:
  DF_StringColumn();
  ~DF_StringColumn() override;

  // Constructs a column by copying the data from the provided column into
  // this new instance of column.
  DF_StringColumn(DF_StringColumn &column);

  DF_StringColumn *as_string() override { return this; }

  /** Returns the string at idx; undefined on invalid idx.*/
  String *get(size_t idx);

  /** Out of bound idx is undefined. */
  void set(size_t idx, String *val);
};

/**
 * Name: Snowy Chen, Joe Song
 * Date: 14 February 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

//lang::CwC

#pragma once

#include <stdarg.h>
#include <stdlib.h>

#include "object.h"
#include "custom_string.h"

#include "dataframe_helper.h"

/**
 * Class prototypes
 */
class IntColumn;
class BoolColumn;
class FloatColumn;
class StringColumn;

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
class Column : public Object {
public:
  // A protected variable containing all of the data within the column. It
  // can be used by its derivative classes.
  LinkedListArray_ *list_;

  // The data type of the column. All inherited columns should set this value
  // to the appropriate column data type upon construction.
  ColumnType_t data_type_;

  // Constructs a column
  explicit Column(ColumnType_t column_type) {
    this->list_ = new LinkedListArray_();
    this->data_type_ = column_type;
  }

  // Deconstructs the column
  ~Column() override { delete this->list_; }

  /** Type converters: Return same column under its actual type, or
   *  nullptr if of the wrong type.  */
  virtual IntColumn *as_int() { return nullptr; }

  virtual BoolColumn *as_bool() { return nullptr; }

  virtual FloatColumn *as_float() { return nullptr; }

  virtual StringColumn *as_string() { return nullptr; }

  /** Type appropriate push_back methods. Calling the wrong method is
   * undefined behavior. **/
  virtual void push_back(int val) {
    // Check if this column is the correct data type for this function. If it
    // is not, do nothing. If it is, push it onto the linked list
    if (this->data_type_ == ColumnType_Integer) {
      // Create the item.
      DataItem_ new_item;
      new_item.i = val;
      this->list_->add_new_item(new_item);
    }
  }

  virtual void push_back(bool val) {
    // Check if this column is the correct data type for this function. If it
    // is not, do nothing. If it is, push it onto the linked list
    if (this->data_type_ == ColumnType_Bool) {
      // Create the item.
      DataItem_ new_item;
      new_item.b = val;
      this->list_->add_new_item(new_item);
    }
  }

  virtual void push_back(float val) {
    // Check if this column is the correct data type for this function. If it
    // is not, do nothing. If it is, push it onto the linked list
    if (this->data_type_ == ColumnType_Float) {
      // Create the item.
      DataItem_ new_item;
      new_item.f = val;
      this->list_->add_new_item(new_item);
    }
  }

  virtual void push_back(String *val) {
    // Check if this column is the correct data type for this function. If it
    // is not, do nothing. If it is, push it onto the linked list
    if (this->data_type_ == ColumnType_String) {
      // Create the item.
      DataItem_ new_item;
      new_item.s = val;
      this->list_->add_new_item(new_item);
    }
  }

  /** Returns the number of elements in the column. */
  virtual size_t size() { return this->list_->size(); }

  /** Return the type of this column as a char: 'S', 'B', 'I' and 'F'.**/
  char get_type() { return static_cast<char>(this->data_type_); }
};

/*************************************************************************
 * BoolColumn::
 * Holds primitive int values, unwrapped.
 */
class BoolColumn : public Column {
public:
  BoolColumn() : Column(ColumnType_Bool) {}

  explicit BoolColumn(int n, ...) : Column(ColumnType_Bool) {
    // Iterate through the variadic list as boolean values and add them into
    // the list within the column
    va_list ap;
    va_start(ap, n);
    for (int i = 0; i < n; i++) {
      // Create the boolean data item and pass it into the linked list of
      // array of items
      bool val = static_cast<bool>(va_arg(ap, int));
      this->push_back(val);
    }
    va_end(ap);
  }

  // Constructs a column by copying the data from the provided column into
  // this new instance of column.
  BoolColumn(BoolColumn &column) : Column(ColumnType_Bool) {
    this->data_type_ = column.data_type_;
    this->list_ = new LinkedListArray_(*column.list_);
  }

  bool get(size_t idx) {
    DataItem_ data = this->list_->get_item(idx);
    return data.b;
  }

  BoolColumn *as_bool() override { return this; }

  void set(size_t idx, bool val) {
    // Create a data item to be passed
    DataItem_ data;
    data.b = val;

    // Set the item
    this->list_->set_new_item(idx, data);
  }

  size_t size() override { return Column::size(); }
};

/*************************************************************************
 * IntColumn::
 * Holds int values.
 */
class IntColumn : public Column {
public:
  IntColumn() : Column(ColumnType_Integer) {}

  explicit IntColumn(int n, ...) : Column(ColumnType_Integer) {
    // Iterate through the variadic list as int values and add them into
    // the list within the column
    va_list ap;
    va_start(ap, n);
    for (int i = 0; i < n; i++) {
      // Create the integer item.
      int val = va_arg(ap, int);
      this->push_back(val);
    }
    va_end(ap);
  }

  // Constructs a column by copying the data from the provided column into
  // this new instance of column.
  IntColumn(IntColumn &column) : Column(ColumnType_Integer) {
      this->data_type_ = column.data_type_;
      this->list_ = new LinkedListArray_(*column.list_);
  }

  int get(size_t idx) {
    DataItem_ data = this->list_->get_item(idx);
    return data.i;
  }

  IntColumn *as_int() override { return this; }

  /** Set value at idx. An out of bound idx is undefined.  */
  void set(size_t idx, int val) {
    // Create a data item to be passed
    DataItem_ data;
    data.i = val;

    // Set the item
    this->list_->set_new_item(idx, data);
  }

  size_t size() override { return Column::size(); }
};

/*************************************************************************
 * FloatColumn::
 * Holds primitive int values, unwrapped.
 */
class FloatColumn : public Column {
public:
  FloatColumn() : Column(ColumnType_Float) {}

  explicit FloatColumn(int n, ...) : Column(ColumnType_Float) {
    // Iterate through the variadic list as float values and add them into
    // the list within the column
    va_list ap;
    va_start(ap, n);
    for (int i = 0; i < n; i++) {
      // Create the float item.
      double val = va_arg(ap, double);
      this->push_back((float)val);
    }
    va_end(ap);
  }

  // Constructs a column by copying the data from the provided column into
  // this new instance of column.
  FloatColumn(FloatColumn &column) : Column(ColumnType_Float) {
      this->data_type_ = column.data_type_;
      this->list_ = new LinkedListArray_(*column.list_);
  }

  float get(size_t idx) {
    DataItem_ data = this->list_->get_item(idx);
    return data.f;
  }

  FloatColumn *as_float() override { return this; }

  void set(size_t idx, float val) {
    // Create a data item to be passed
    DataItem_ data;
    data.f = val;

    // Set the item
    this->list_->set_new_item(idx, data);
  }

  size_t size() override { return Column::size(); }
};

/*************************************************************************
 * StringColumn::
 * Holds string pointers. The strings are external.  Nullptr is a valid
 * value.
 */
class StringColumn : public Column {
public:
  StringColumn() : Column(ColumnType_String) {}

  explicit StringColumn(int n, ...) : Column(ColumnType_String) {
    // Iterate through the variadic list as string values and add them into
    // the list within the column
    va_list ap;
    va_start(ap, n);
    for (int i = 0; i < n; i++) {
      // Create the float item.
      String *val = va_arg(ap, String *);
      this->push_back(val);
    }
    va_end(ap);
  }

  // Constructs a column by copying the data from the provided column into
  // this new instance of column.
  StringColumn(StringColumn &column) : Column(ColumnType_String) {
      this->data_type_ = column.data_type_;
      this->list_ = new LinkedListArray_(*column.list_);
  }

  StringColumn *as_string() override { return this; }

  /** Returns the string at idx; undefined on invalid idx.*/
  String *get(size_t idx) {
    DataItem_ data = this->list_->get_item(idx);
    return data.s;
  }

  /** Out of bound idx is undefined. */
  void set(size_t idx, String *val) {
    // Create a data item to be passed
    DataItem_ data;
    data.s = val;

    // Set the item
    this->list_->set_new_item(idx, data);
  }

  size_t size() override { return Column::size(); }
};

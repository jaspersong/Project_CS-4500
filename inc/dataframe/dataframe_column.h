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
#include <vector>

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
  DF_Column(DF_Column &original);

  // Deconstructs the column
  ~DF_Column() override;

  /** Type appropriate push_back methods. Calling the wrong method is
   * undefined behavior. **/
  void push_back(int val);
  void push_back(bool val);
  void push_back(float val);
  void push_back(String *val);

  bool get_bool(size_t idx);
  int get_int(size_t idx);
  float get_float(size_t idx);
  String *get_string(size_t idx);

  void set(size_t idx, bool val);
  void set(size_t idx, int val);
  void set(size_t idx, float val);
  void set(size_t idx, String *val); // String remains external

  /** Returns the number of elements in the column. */
  size_t size() { return this->values.size(); }

  /** Return the type of this column as a char: 'S', 'B', 'I' and 'F'.**/
  ColumnType_t get_type() { return this->data_type; }

private:
  typedef union {
    bool b;
    int i;
    float f;
    String *s;
  } DataItem;

  std::vector<DataItem> values;

  ColumnType_t data_type;
};

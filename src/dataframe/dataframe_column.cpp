/**
 * Name: Snowy Chen, Joe Song
 * Date: 21 March 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::Cpp

#include "dataframe_column.h"

DF_Column::DF_Column(ColumnType_t column_type) {
  this->list = new ArrayOfArrays();
  this->data_type = column_type;
}

DF_Column::~DF_Column() { delete this->list; }

void DF_Column::push_back(int val) {
  // Check if this column is the correct data type for this function. If it
  // is not, do nothing. If it is, push it onto the linked list
  if (this->data_type == ColumnType_Integer) {
    // Create the item.
    DataItem_ new_item;
    new_item.i = val;
    this->list->add_new_item(new_item);
  }
}

void DF_Column::push_back(bool val) {
  // Check if this column is the correct data type for this function. If it
  // is not, do nothing. If it is, push it onto the linked list
  if (this->data_type == ColumnType_Bool) {
    // Create the item.
    DataItem_ new_item;
    new_item.b = val;
    this->list->add_new_item(new_item);
  }
}

void DF_Column::push_back(float val) {
  // Check if this column is the correct data type for this function. If it
  // is not, do nothing. If it is, push it onto the linked list
  if (this->data_type == ColumnType_Float) {
    // Create the item.
    DataItem_ new_item;
    new_item.f = val;
    this->list->add_new_item(new_item);
  }
}

void DF_Column::push_back(String *val) {
  // Check if this column is the correct data type for this function. If it
  // is not, do nothing. If it is, push it onto the linked list
  if (this->data_type == ColumnType_String) {
    // Create the item.
    DataItem_ new_item;
    new_item.s = val;
    this->list->add_new_item(new_item);
  }
}

size_t DF_Column::size() { return this->list->size(); }

char DF_Column::get_type() { return static_cast<char>(this->data_type); }

/*************************************************************************/

DF_BoolColumn::DF_BoolColumn() : DF_Column(ColumnType_Bool) {}

DF_BoolColumn::DF_BoolColumn(int n, ...) : DF_Column(ColumnType_Bool) {
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

DF_BoolColumn::DF_BoolColumn(DF_BoolColumn &column)
    : DF_Column(ColumnType_Bool) {
  this->data_type = column.data_type;
  delete this->list;
  this->list = new ArrayOfArrays(*column.list);
}

bool DF_BoolColumn::get(size_t idx) {
  DataItem_ data = this->list->get_item(idx);
  return data.b;
}

void DF_BoolColumn::set(size_t idx, bool val) {
  // Create a data item to be passed
  DataItem_ data;
  data.b = val;

  // Set the item
  this->list->set_new_item(idx, data);
}

/*************************************************************************/

DF_IntColumn::DF_IntColumn() : DF_Column(ColumnType_Integer) {}

DF_IntColumn::DF_IntColumn(int n, ...) : DF_Column(ColumnType_Integer) {
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

DF_IntColumn::DF_IntColumn(DF_IntColumn &column)
    : DF_Column(ColumnType_Integer) {
  this->data_type = column.data_type;
  delete this->list;
  this->list = new ArrayOfArrays(*column.list);
}

int DF_IntColumn::get(size_t idx) {
  DataItem_ data = this->list->get_item(idx);
  return data.i;
}

void DF_IntColumn::set(size_t idx, int val) {
  // Create a data item to be passed
  DataItem_ data;
  data.i = val;

  // Set the item
  this->list->set_new_item(idx, data);
}

/*************************************************************************/

DF_FloatColumn::DF_FloatColumn() : DF_Column(ColumnType_Float) {}

DF_FloatColumn::DF_FloatColumn(int n, ...) : DF_Column(ColumnType_Float) {
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

DF_FloatColumn::DF_FloatColumn(DF_FloatColumn &column)
    : DF_Column(ColumnType_Float) {
  this->data_type = column.data_type;
  delete this->list;
  this->list = new ArrayOfArrays(*column.list);
}

float DF_FloatColumn::get(size_t idx) {
  DataItem_ data = this->list->get_item(idx);
  return data.f;
}

void DF_FloatColumn::set(size_t idx, float val) {
  // Create a data item to be passed
  DataItem_ data;
  data.f = val;

  // Set the item
  this->list->set_new_item(idx, data);
}

/*************************************************************************/

DF_StringColumn::DF_StringColumn() : DF_Column(ColumnType_String) {}

DF_StringColumn::DF_StringColumn(int n, ...) : DF_Column(ColumnType_String) {
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

DF_StringColumn::DF_StringColumn(DF_StringColumn &column)
    : DF_Column(ColumnType_String) {
  this->data_type = column.data_type;
  delete this->list;
  this->list = new ArrayOfArrays(*column.list);
}

String *DF_StringColumn::get(size_t idx) {
  DataItem_ data = this->list->get_item(idx);
  return data.s;
}

void DF_StringColumn::set(size_t idx, String *val) {
  // Create a data item to be passed
  DataItem_ data;
  data.s = val;

  // Set the item
  this->list->set_new_item(idx, data);
}

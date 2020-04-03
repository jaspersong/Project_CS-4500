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
    if (val != nullptr) {
      new_item.s = new String(*val);
    } else {
      new_item.s = nullptr;
    }
    this->list->add_new_item(new_item);
  }
}

size_t DF_Column::size() { return this->list->size(); }

char DF_Column::get_type() { return static_cast<char>(this->data_type); }

/*************************************************************************/

DF_BoolColumn::DF_BoolColumn() : DF_Column(ColumnType_Bool) {}

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

DF_StringColumn::~DF_StringColumn() {
  for (size_t i = 0; i < this->list->size(); i++) {
    delete this->get(i);
  }
}

DF_StringColumn::DF_StringColumn(DF_StringColumn &column)
    : DF_Column(ColumnType_String) {
  this->data_type = column.data_type;
  for (size_t i = 0; i < column.size(); i++) {
    DataItem_ data;
    String *val = column.get(i);
    if (val != nullptr) {
      data.s = new String(*val);
    } else {
      data.s = nullptr;
    }
    this->list->add_new_item(data);
  }
}

String *DF_StringColumn::get(size_t idx) {
  DataItem_ data = this->list->get_item(idx);
  return data.s;
}

void DF_StringColumn::set(size_t idx, String *val) {
  // Create a data item to be passed
  DataItem_ data;
  if (val != nullptr) {
    data.s = new String(*val);
  } else {
    data.s = nullptr;
  }

  // Replace the old string with the new one
  String *old_str = this->get(idx);
  delete old_str;
  this->list->set_new_item(idx, data);
}

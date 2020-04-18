/**
 * Name: Snowy Chen, Joe Song
 * Date: 21 March 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::Cpp

#include "dataframe_column.h"

DF_Column::DF_Column(ColumnType_t column_type) {
  this->data_type = column_type;
}

DF_Column::DF_Column(DF_Column &original) : CustomObject(original) {
  this->data_type = original.data_type;
  this->values.clear();
  for (auto orig_item : original.values) {
    switch (original.data_type) {
    case ColumnType_Bool:
      this->push_back(orig_item.b);
      break;
    case ColumnType_Integer:
      this->push_back(orig_item.i);
      break;
    case ColumnType_Float:
      this->push_back(orig_item.f);
      break;
    case ColumnType_String:
    default:
      this->push_back(orig_item.s);
      break;
    }
  }
}

DF_Column::~DF_Column() {
  // Delete only if it's a string value
  if (this->data_type == ColumnType_String) {
    for (auto item : this->values) {
      delete item.s;
    }
  }
}

void DF_Column::push_back(int val) {
  assert(this->data_type == ColumnType_Integer);

  DataItem new_item;
  new_item.i = val;
  this->values.push_back(new_item);
}

void DF_Column::push_back(bool val) {
  assert(this->data_type == ColumnType_Bool);

  DataItem new_item;
  new_item.b = val;
  this->values.push_back(new_item);
}

void DF_Column::push_back(float val) {
  assert(this->data_type == ColumnType_Float);

  DataItem new_item;
  new_item.f = val;
  this->values.push_back(new_item);
}

void DF_Column::push_back(String *val) {
  assert(this->data_type == ColumnType_String);

  DataItem new_item;
  if (val != nullptr) {
    new_item.s = new String(*val);
  } else {
    new_item.s = nullptr;
  }
  this->values.push_back(new_item);
}
bool DF_Column::get_bool(size_t idx) {
  assert(this->data_type == ColumnType_Bool);
  DataItem item = this->values.at(idx);
  return item.b;
}

int DF_Column::get_int(size_t idx) {
  assert(this->data_type == ColumnType_Integer);
  DataItem item = this->values.at(idx);
  return item.i;
}

float DF_Column::get_float(size_t idx) {
  assert(this->data_type == ColumnType_Float);
  DataItem item = this->values.at(idx);
  return item.f;
}

String *DF_Column::get_string(size_t idx) {
  assert(this->data_type == ColumnType_String);
  DataItem item = this->values.at(idx);
  return item.s;
}

void DF_Column::set(size_t idx, bool val) {
  assert(this->data_type == ColumnType_Bool);
  DataItem new_item;
  new_item.b = val;
  this->values[idx] = new_item;
}

void DF_Column::set(size_t idx, int val) {
  assert(this->data_type == ColumnType_Integer);
  DataItem new_item;
  new_item.i = val;
  this->values[idx] = new_item;
}

void DF_Column::set(size_t idx, float val) {
  assert(this->data_type == ColumnType_Float);
  DataItem new_item;
  new_item.f = val;
  this->values[idx] = new_item;
}

void DF_Column::set(size_t idx, String *val) {
  assert(this->data_type == ColumnType_String);

  // Delete the old value
  DataItem value = this->values.at(idx);
  delete value.s;

  // Add the new value
  if (val != nullptr) {
    value.s = new String(*val);
  } else {
    value.s = nullptr;
  }
  this->values[idx] = value;
}

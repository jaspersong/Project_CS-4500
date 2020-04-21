/**
 * Name: Snowy Chen, Joe Song
 * Date: 21 March 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::Cpp

#include "dataframe.h"

DataFrame::DataFrame(DataFrame &df) : DataFrame(df.get_schema()) {}

DataFrame::DataFrame(Schema &schema) : schema(schema) {
  this->num_rows = 0;

  // add blank internal columns
  for (size_t i = 0; i < schema.width(); i++) {
    // Create blank columns and add them to the list accordingly
    this->col_list.push_back(new DF_Column(schema.col_type(i)));
  }
}

DataFrame::~DataFrame() {
  for (auto &i : this->col_list) {
    delete i;
  }
}

Schema &DataFrame::get_schema() { return this->schema; }

void DataFrame::add_column(DF_Column *col) {
  // Verify that the column is valid
  if (col == nullptr) {
    printf("Invalid column provided to the dataframe.\n");
    assert(false);
  } else {
    // Add the new column information to the schema
    this->schema.add_column(col->get_type());

    // Copy the new column and add it to the column list
    this->col_list.push_back(new DF_Column(*col));

    // Update the number of rows accordingly
    if (this->num_rows < col->size()) {
      this->num_rows = col->size();
    }
  }
}

int DataFrame::get_int(size_t col, size_t row) {
  return this->col_list.at(col)->get_int(row);
}

bool DataFrame::get_bool(size_t col, size_t row) {
  return this->col_list.at(col)->get_bool(row);
}

float DataFrame::get_float(size_t col, size_t row) {
  return this->col_list.at(col)->get_float(row);
}

String *DataFrame::get_string(size_t col, size_t row) {
  return this->col_list.at(col)->get_string(row);
}

void DataFrame::set(size_t col, size_t row, int val) {
  this->col_list.at(col)->set(row, val);
}

void DataFrame::set(size_t col, size_t row, bool val) {
  this->col_list.at(col)->set(row, val);
}

void DataFrame::set(size_t col, size_t row, float val) {
  this->col_list.at(col)->set(row, val);
}

void DataFrame::set(size_t col, size_t row, String *val) {
  this->col_list.at(col)->set(row, val);
}

void DataFrame::fill_row(size_t idx, Row &row) {
  // NOTE: This function will add a new row in order to accomodate the
  // provided row index, if the index is out of bounds for this data frame
  // at the time. add_row() will be taking advantage of this functionality.

  // Verify that the row follows the same schema
  assert(row.width() == this->schema.width());
  // Iterate through the row fields and make sure that the column data
  // types match with the schema
  for (size_t i = 0; i < row.width(); i++) {
    assert(row.col_type(i) == this->schema.col_type(i));
  }

  // Iterate down the row to fill the column values at the specified row index
  for (size_t i = 0; i < row.width(); i++) {
    // Get the column at the specified index
    auto column = this->col_list.at(i);

    switch (column->get_type()) {
    case ColumnType_Bool:
      // Add 0 until we reach the row index so we may set the value
      while (idx >= column->size()) {
        column->push_back(false);
      }

      // Now set the value from the row to this column
      column->set(idx, row.get_bool(i));
      break;
    case ColumnType_Integer:
      // Add 0 until we reach the row index so we may set the value
      while (idx >= column->size()) {
        column->push_back(0);
      }

      // Now set the value from the row to this column
      column->set(idx, row.get_int(i));
      break;
    case ColumnType_Float:
      // Add 0 until we reach the row index so we may set the value
      while (idx >= column->size()) {
        column->push_back(0.0f);
      }

      // Now set the value from the row to this column
      column->set(idx, row.get_float(i));
      break;
    case ColumnType_String:
    default:
      // Add 0 until we reach the row index so we may set the value
      while (idx >= column->size()) {
        column->push_back(nullptr);
      }

      // Now set the value from the row to this column
      column->set(idx, row.get_string(i));
      break;
    }
  }

  // Update the number of rows if the index provided adds more rows to this
  // data frame
  if (idx + 1 > this->num_rows) {
    this->num_rows = idx + 1;
  }
}

void DataFrame::add_row(Row &row) {
  // Call fill row because it will automatically add rows to the columns in
  // order to accomodate the provided new row
  this->fill_row(this->nrows(), row);
}

size_t DataFrame::nrows() { return this->num_rows; }

size_t DataFrame::ncols() { return this->schema.width(); }

Row *DataFrame::copy_row(size_t row) {
  Row *new_row = new Row(this->schema);

  // Go through the columns and copy the data into the row
  for (size_t c = 0; c < this->ncols(); c++) {
    // Get the column at the specified index
    auto column = this->col_list.at(c);

    // Handle adding to the rows based off of the type of the column
    switch (column->get_type()) {
    case ColumnType_Bool:
      new_row->set(c, column->get_bool(row));
      break;
    case ColumnType_Integer:
      new_row->set(c, column->get_int(row));
      break;
    case ColumnType_Float:
      new_row->set(c, column->get_float(row));
      break;
    case ColumnType_String:
    default:
      new_row->set(c, column->get_string(row));
      break;
    }
  }

  return new_row;
}

void DataFrame::map(Rower &r) {
  // Iterate down the rows of the dataframe
  for (size_t row = 0; row < this->nrows(); row++) {
    // Create a from the row index
    Row *new_row = this->copy_row(row);

    // Put the row through the rower
    r.accept(*new_row);

    // Delete the row now that we put it through the rower
    delete new_row;
  }
}

void DataFrame::serialize(Serializer &serializer) {
  serializer.set_size_t(this->ncols());
  serializer.set_size_t(this->nrows());

  for (size_t c = 0; c < this->ncols(); c++) {
    ColumnType_t data_type = this->get_schema().col_type(c);
    serializer.set_generic(reinterpret_cast<unsigned char *>(&data_type),
                           sizeof(data_type));
    for (size_t r = 0; r < this->nrows(); r++) {
      switch (data_type) {
      case ColumnType_Bool:
        serializer.set_bool(this->get_bool(c, r));
        break;
      case ColumnType_Integer:
        serializer.set_int(this->get_int(c, r));
        break;
      case ColumnType_Float:
        serializer.set_double(this->get_float(c, r));
        break;
      case ColumnType_String:
      default:
        this->get_string(c, r)->serialize(serializer);
        break;
      }
    }
  }
}

size_t DataFrame::serialization_required_bytes() {
  size_t ret_value = Serializer::get_required_bytes(this->ncols());
  ret_value += Serializer::get_required_bytes(this->nrows());

  for (size_t c = 0; c < this->ncols(); c++) {
    ColumnType_t data_type = this->get_schema().col_type(c);
    ret_value += sizeof(ColumnType_t);
    for (size_t r = 0; r < this->nrows(); r++) {
      switch (data_type) {
      case ColumnType_Bool:
        ret_value += Serializer::get_required_bytes(this->get_bool(c, r));
        break;
      case ColumnType_Integer:
        ret_value += Serializer::get_required_bytes(this->get_int(c, r));
        break;
      case ColumnType_Float:
        ret_value += Serializer::get_required_bytes(
            static_cast<double>(this->get_float(c, r)));
        break;
      case ColumnType_String:
      default:
        ret_value += this->get_string(c, r)->serialization_required_bytes();
        break;
      }
    }
  }

  return ret_value;
}

DataFrame *DataFrame::deserialize_as_dataframe(Deserializer &deserializer) {
  size_t num_cols = deserializer.get_size_t();
  size_t num_rows = deserializer.get_size_t();

  Schema schema;
  auto *df = new DataFrame(schema);

  for (size_t c = 0; c < num_cols; c++) {
    // Get the column type
    ColumnType_t data_type;
    auto *type_bytes = reinterpret_cast<unsigned char *>(&data_type);
    for (size_t i = 0; i < sizeof(ColumnType_t); i++) {
      type_bytes[i] = deserializer.get_byte();
    }

    // Get the column values
    DF_Column column(data_type);
    switch (data_type) {
    case 'B':
      for (size_t r = 0; r < num_rows; r++) {
        bool value = deserializer.get_bool();
        column.push_back(value);
      }
      break;
    case 'I':
      for (size_t r = 0; r < num_rows; r++) {
        int value = deserializer.get_int();
        column.push_back(value);
      }
      break;
    case 'F':
      for (size_t r = 0; r < num_rows; r++) {
        auto value = (float)deserializer.get_double();
        column.push_back(value);
      }
      break;
    case 'S':
    default:
      for (size_t r = 0; r < num_rows; r++) {
        String *value = String::deserialize_as_string(deserializer);
        column.push_back(value);
        delete value; // There is no need for this anymore.
      }
      break;
    }

    df->add_column(&column);
  }

  return df;
}

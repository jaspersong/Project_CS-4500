/**
 * Name: Snowy Chen, Joe Song
 * Date: 21 March 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::Cpp

#include "dataframe.h"

DataFrame::DataFrame(DataFrame &df) : DataFrame(df.get_schema()) {}

DataFrame::DataFrame(Schema &schema) {
  this->col_list = new ArrayOfArrays();
  this->num_rows = 0;
  this->schema = new Schema(schema);

  // add blank internal columns
  for (size_t i = 0; i < schema.width(); i++) {
    // Create blank columns and add them to the list accordingly
    DF_Column *new_column = nullptr;
    switch (schema.col_type(i)) {
    case ColumnType_Bool:
      new_column = new DF_BoolColumn();
      break;
    case ColumnType_Integer:
      new_column = new DF_IntColumn();
      break;
    case ColumnType_Float:
      new_column = new DF_FloatColumn();
      break;
    case ColumnType_String:
    default:
      new_column = new DF_StringColumn();
      break;
    }
    DataItem_ item;
    item.o = new_column;
    this->col_list->add_new_item(item);
  }
}

DataFrame::~DataFrame() {
  delete this->schema;

  // Iterate through the column and delete the internally allocated columns
  for (size_t i = 0; i < this->col_list->size(); i++) {
    DataItem_ item = this->col_list->get_item(i);
    auto *column = dynamic_cast<DF_Column *>(item.o);
    delete column;
  }
  delete this->col_list;
}

Schema &DataFrame::get_schema() { return *this->schema; }

void DataFrame::add_column(DF_Column *col) {
  // Verify that the column is valid
  if (col == nullptr) {
    printf("Invalid column provided to the dataframe.\n");
    exit(1);
  } else {
    // Add the new column information to the schema
    this->schema->add_column(col->get_type());

    // Copy the new column and add it to the column list
    DF_Column *new_column = nullptr;
    switch (col->get_type()) {
    case ColumnType_Bool:
      new_column = new DF_BoolColumn(*col->as_bool());
      break;
    case ColumnType_Integer:
      new_column = new DF_IntColumn(*col->as_int());
      break;
    case ColumnType_Float:
      new_column = new DF_FloatColumn(*col->as_float());
      break;
    case ColumnType_String:
    default:
      new_column = new DF_StringColumn(*col->as_string());
      break;
    }
    DataItem_ item;
    item.o = new_column;
    this->col_list->add_new_item(item);

    // Update the number of rows accordingly
    if (this->num_rows < new_column->size()) {
      this->num_rows = new_column->size();
    }
  }
}

bool DataFrame::verify_col_row_parameters(size_t col, size_t row,
                                           ColumnType_t type) {
  if ((row < 0) || (row > this->nrows())) {
    printf("Row %zu is out of bounds.\n", row);
    return false;
  } else if ((col < 0) || (col > this->ncols())) {
    printf("Column %zu is out of bounds.\n", col);
    return false;
  } else if (this->schema->col_type(col) != static_cast<char>(type)) {
    printf("Column %zu is not the correct datatype.\n", col);
    return false;
  } else {
    return true;
  }
}

int DataFrame::get_int(size_t col, size_t row) {
  // Verify that these values are within bounds
  if (!this->verify_col_row_parameters(col, row, ColumnType_Integer)) {
    exit(1);
  } else {
    // Get the column
    DataItem_ item = this->col_list->get_item(col);
    auto *column = dynamic_cast<DF_Column *>(item.o);

    // Now get the item in that row for that column
    DF_IntColumn *int_column = column->as_int();
    return int_column->get(row);
  }
}

bool DataFrame::get_bool(size_t col, size_t row) {
  // Verify that these values are within bounds
  if (!this->verify_col_row_parameters(col, row, ColumnType_Bool)) {
    exit(1);
  } else {
    // Get the column
    DataItem_ item = this->col_list->get_item(col);
    auto *column = dynamic_cast<DF_Column *>(item.o);

    // Now get the item in that row for that column
    auto *bool_column = dynamic_cast<DF_BoolColumn *>(column);
    return bool_column->get(row);
  }
}

float DataFrame::get_float(size_t col, size_t row) {
  // Verify that these values are within bounds
  if (!this->verify_col_row_parameters(col, row, ColumnType_Float)) {
    exit(1);
  } else {
    // Get the column
    DataItem_ item = this->col_list->get_item(col);
    auto *column = dynamic_cast<DF_Column *>(item.o);

    // Now get the item in that row for that column
    DF_FloatColumn *float_column = column->as_float();
    return float_column->get(row);
  }
}

String *DataFrame::get_string(size_t col, size_t row) {
  // Verify that these values are within bounds
  if (!this->verify_col_row_parameters(col, row, ColumnType_String)) {
    exit(1);
  } else {
    // Get the column
    DataItem_ item = this->col_list->get_item(col);
    auto *column = dynamic_cast<DF_Column *>(item.o);

    // Now get the item in that row for that column
    DF_StringColumn *string_column = column->as_string();
    return string_column->get(row);
  }
}

void DataFrame::set(size_t col, size_t row, int val) {
  // Verify that these values are within bounds
  if (!this->verify_col_row_parameters(col, row, ColumnType_Integer)) {
    exit(1);
  } else {
    // Get the column
    DataItem_ item = this->col_list->get_item(col);
    auto *column = dynamic_cast<DF_Column *>(item.o);

    // Now get the item in that row for that column
    DF_IntColumn *int_column = column->as_int();
    int_column->set(row, val);
  }
}

void DataFrame::set(size_t col, size_t row, bool val) {
  // Verify that these values are within bounds
  if (!this->verify_col_row_parameters(col, row, ColumnType_Bool)) {
    exit(1);
  } else {
    // Get the column
    DataItem_ item = this->col_list->get_item(col);
    auto *column = dynamic_cast<DF_Column *>(item.o);

    // Now get the item in that row for that column
    DF_BoolColumn *bool_column = column->as_bool();
    bool_column->set(row, val);
  }
}

void DataFrame::set(size_t col, size_t row, float val) {
  // Verify that these values are within bounds
  if (!this->verify_col_row_parameters(col, row, ColumnType_Float)) {
    exit(1);
  } else {
    // Get the column
    DataItem_ item = this->col_list->get_item(col);
    auto *column = dynamic_cast<DF_Column *>(item.o);

    // Now get the item in that row for that column
    DF_FloatColumn *float_column = column->as_float();
    float_column->set(row, val);
  }
}

void DataFrame::set(size_t col, size_t row, String *val) {
  // Verify that these values are within bounds
  if (!this->verify_col_row_parameters(col, row, ColumnType_String)) {
    exit(1);
  } else {
    // Get the column
    DataItem_ item = this->col_list->get_item(col);
    auto *column = dynamic_cast<DF_Column *>(item.o);

    // Now get the item in that row for that column
    DF_StringColumn *stringColumn = column->as_string();
    stringColumn->set(row, val);
  }
}

void DataFrame::fill_row(size_t idx, Row &row) {
  // NOTE: This function will add a new row in order to accomodate the
  // provided row index, if the index is out of bounds for this data frame
  // at the time. add_row() will be taking advantage of this functionality.

  // Verify that the row follows the same schema
  if (row.width() != this->schema->width()) {
    printf("Row provided does not follow the same schema.\n");
    exit(1);
  }
  // Iterate through the row fields and make sure that the column data
  // types match with the schema
  for (size_t i = 0; i < row.width(); i++) {
    if (row.col_type(i) != this->schema->col_type(i)) {
      printf("Row provided does not follow the same schema.\n");
      exit(1);
    }
  }

  // Iterate down the row to fill the column values at the specified row index
  for (size_t i = 0; i < row.width(); i++) {
    // Get the column at the specified index
    DataItem_ item = this->col_list->get_item(i);
    auto *column = dynamic_cast<DF_Column *>(item.o);

    switch (column->get_type()) {
    case ColumnType_Bool: {
      DF_BoolColumn *bool_column = column->as_bool();

      // Add 0 until we reach the row index so we may set the value
      while (idx >= bool_column->size()) {
        bool_column->push_back(false);
      }

      // Now set the value from the row to this column
      bool_column->set(idx, row.get_bool(i));
      break;
    }
    case ColumnType_Integer: {
      DF_IntColumn *int_column = column->as_int();

      // Add 0 until we reach the row index so we may set the value
      while (idx >= int_column->size()) {
        int_column->push_back(0);
      }

      // Now set the value from the row to this column
      int_column->set(idx, row.get_int(i));
      break;
    }
    case ColumnType_Float: {
      DF_FloatColumn *float_column = column->as_float();

      // Add 0 until we reach the row index so we may set the value
      while (idx >= float_column->size()) {
        float_column->push_back(0.0f);
      }

      // Now set the value from the row to this column
      float_column->set(idx, row.get_float(i));
      break;
    }
    case ColumnType_String:
    default: {
      DF_StringColumn *string_column = column->as_string();

      // Add 0 until we reach the row index so we may set the value
      while (idx >= string_column->size()) {
        string_column->push_back(nullptr);
      }

      // Now set the value from the row to this column
      string_column->set(idx, row.get_string(i));
      break;
    }
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

size_t DataFrame::ncols() { return this->schema->width(); }

Row *DataFrame::create_row(size_t row) {
  Row *new_row = new Row(*this->schema);

  // Go through the columns and copy the data into the row
  for (size_t c = 0; c < this->ncols(); c++) {
    // Get the column at the specified index
    DataItem_ item = this->col_list->get_item(c);
    auto *column = dynamic_cast<DF_Column *>(item.o);

    // Handle adding to the rows based off of the type of the column
    switch (column->get_type()) {
    case ColumnType_Bool: {
      DF_BoolColumn *bool_column = column->as_bool();
      new_row->set(c, bool_column->get(row));
      break;
    }
    case ColumnType_Integer: {
      DF_IntColumn *int_column = column->as_int();
      new_row->set(c, int_column->get(row));
      break;
    }
    case ColumnType_Float: {
      DF_FloatColumn *float_column = column->as_float();
      new_row->set(c, float_column->get(row));
      break;
    }
    case ColumnType_String:
    default: {
      DF_StringColumn *string_column = column->as_string();
      new_row->set(c, string_column->get(row));
    }
    }
  }

  new_row->set_idx(row);

  return new_row;
}

void DataFrame::map(Rower &r) {
  // Iterate down the rows of the dataframe
  for (size_t row = 0; row < this->nrows(); row++) {
    // Create a from the row index
    Row *new_row = this->create_row(row);

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
    char data_type = this->get_schema().col_type(c);
    serializer.set_generic(reinterpret_cast<unsigned char *>(&data_type),
        sizeof(data_type));
    for (size_t r = 0; r < this->nrows(); r++) {
      switch (data_type) {
      case 'B':
        serializer.set_bool(this->get_bool(c, r));
        break;
      case 'I':
        serializer.set_int(this->get_int(c, r));
        break;
      case 'F':
        serializer.set_double(this->get_float(c, r));
        break;
      case 'S':
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
    char data_type = this->get_schema().col_type(c);
    ret_value += 1;
    for (size_t r = 0; r < this->nrows(); r++) {
      switch (data_type) {
      case 'B':
        ret_value += Serializer::get_required_bytes(this->get_bool(c, r));
        break;
      case 'I':
        ret_value += Serializer::get_required_bytes(this->get_int(c, r));
        break;
      case 'F':
        ret_value += Serializer::get_required_bytes(this->get_float(c, r));
        break;
      case 'S':
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
    char data_type = (char)deserializer.get_byte();
    switch (data_type) {
    case 'B': {
      DF_BoolColumn col;
      for (size_t r = 0; r < num_rows; r++) {
        bool value = deserializer.get_bool();
        col.push_back(value);
      }
      df->add_column(&col);
      break;
    }
    case 'I': {
      DF_IntColumn col;
      for (size_t r = 0; r < num_rows; r++) {
        int value = deserializer.get_int();
        col.push_back(value);
      }
      df->add_column(&col);
      break;
    }
    case 'F': {
      DF_FloatColumn col;
      for (size_t r = 0; r < num_rows; r++) {
        auto value = (float)deserializer.get_double();
        col.push_back(value);
      }
      df->add_column(&col);
      break;
    }
    case 'S':
    default: {
      DF_StringColumn col;
      for (size_t r = 0; r < num_rows; r++) {
        String *value = String::deserialize_as_string(deserializer);
        col.push_back(value);
        // TODO: Need to deconstruct the strings when done
      }
      df->add_column(&col);
      break;
    }
    }
  }

  return df;
}

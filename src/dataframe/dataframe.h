/**
 * Name: Snowy Chen, Joe Song
 * Date: 14 February 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// lang::CwC

#pragma once

#include <application/application.h>
#include <stdlib.h>

#include "custom_object.h"
#include "custom_string.h"
#include "thread.h"

#include "dataframe_column.h"
#include "fielder.h"
#include "print_rower.h"
#include "row.h"
#include "rower.h"
#include "schema.h"

// Forward declaration
class DataFrame;

/**
 * An implementation of thread that will be used to concurrently iterate
 * through the rows of a dataframe. A RowerThread_ will only every other
 * RowerThread_ id row index within the DataFrame.
 */
class RowerThread_ : public Thread {
public:
  DataFrame *df_;
  Rower *rower_;
  size_t id_;
  unsigned inc_;

  /**
   * Constructs a rower thread where the id is the value of all of the
   * indexes this rower thread would handle within the provided DataFrame.
   * @param df The dataframe that the rower thread will iterate through.
   * @param rower The rower that this thread will be rowing with. It should
   *        be an entirely new instance of a rower from all the other threads
   *        in order to prevent data races.
   * @param id The id of the thread. This will be the starting row index that
   *           the thread will start from in the dataframe.
   * @param inc The increment of the row index this thread will be handling.
   *            This means that starting from id, the thread will handle every
   *            inc row index.
   */
  RowerThread_(DataFrame &df, Rower &rower, size_t id, unsigned inc);

  void run() override;
};

/****************************************************************************
 * DataFrame::
 *
 * A DataFrame is table composed of columns of equal length. Each column
 * holds values of the same type (I, S, B, F). A dataframe has a schema that
 * describes it.
 */
class DataFrame : public CustomObject {
public:
  Schema *schema_;

  // ASSERT: This column list will always have the same data type as the
  // schema at any given 0-indexed column index within the bounds of the
  // schema width.
  ArrayOfArrays *col_list_;

  // Cache of the number of rows
  size_t num_rows;

  /** Create a data frame with the same columns as the give df but no rows */
  DataFrame(DataFrame &df) : DataFrame(df.get_schema()) {}

  /** Create a data frame from a schema and columns. All columns are created
   * empty. */
  explicit DataFrame(Schema &schema) {
    this->col_list_ = new ArrayOfArrays();
    this->num_rows = 0;
    this->schema_ = new Schema(schema);

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
      this->col_list_->add_new_item(item);
    }
  }

  /**
   * Deconstructs the dataframe
   */
  ~DataFrame() override {
    delete this->schema_;

    // Iterate through the column and delete the internally allocated columns
    for (size_t i = 0; i < this->col_list_->size(); i++) {
      DataItem_ item = this->col_list_->get_item(i);
      DF_Column *column = dynamic_cast<DF_Column *>(item.o);
      delete column;
    }
  }

  /** Returns the dataframe's schema. Modifying the schema after a dataframe
   * has been created in undefined. */
  Schema &get_schema() { return *this->schema_; }

  /** Adds a column this dataframe, updates the schema, the new column
   * is external, and appears as the last column of the dataframe, the
   * name is optional and external. A nullptr colum is undefined. */
  void add_column(DF_Column *col) {
    // Verify that the column is valid
    if (col == nullptr) {
      printf("Invalid column provided to the dataframe.\n");
      exit(1);
    } else {
      // Add the new column information to the schema
      this->schema_->add_column(col->get_type());

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
      this->col_list_->add_new_item(item);
    }
  }

  /**
   * A helper function that takes in a column, row and column type and
   * verifies that the given coordinate is valid and can execute functions
   * related to the column data type provided.
   * @param col 0-indexed column index.
   * @param row 0-indexed row index.
   * @param type The expected column data type.
   * @return True if everything is invalid. False if otherwise. If it is
   *         false, it will also print out the error message to stdout.
   */
  bool verify_col_row_parameters_(size_t col, size_t row, ColumnType_t type) {
    if ((row < 0) || (row > this->nrows())) {
      printf("Row %zu is out of bounds.\n", row);
      return false;
    } else if ((col < 0) || (col > this->ncols())) {
      printf("Column %zu is out of bounds.\n", col);
      return false;
    } else if (this->schema_->col_type(col) != static_cast<char>(type)) {
      printf("Column %zu is not the correct datatype.\n", col);
      return false;
    } else {
      return true;
    }
  }

  /** Return the value at the given column and row. Accessing rows or
   *  columns out of bounds, or request the wrong type is undefined.*/
  int get_int(size_t col, size_t row) {
    // Verify that these values are within bounds
    if (!this->verify_col_row_parameters_(col, row, ColumnType_Integer)) {
      exit(1);
    } else {
      // Get the column
      DataItem_ item = this->col_list_->get_item(col);
      DF_Column *column = dynamic_cast<DF_Column *>(item.o);

      // Now get the item in that row for that column
      DF_IntColumn *int_column = column->as_int();
      return int_column->get(row);
    }
  }

  bool get_bool(size_t col, size_t row) {
    // Verify that these values are within bounds
    if (!this->verify_col_row_parameters_(col, row, ColumnType_Bool)) {
      exit(1);
    } else {
      // Get the column
      DataItem_ item = this->col_list_->get_item(col);
      DF_Column *column = dynamic_cast<DF_Column *>(item.o);

      // Now get the item in that row for that column
      DF_BoolColumn *bool_column = dynamic_cast<DF_BoolColumn *>(column);
      return bool_column->get(row);
    }
  }

  float get_float(size_t col, size_t row) {
    // Verify that these values are within bounds
    if (!this->verify_col_row_parameters_(col, row, ColumnType_Float)) {
      exit(1);
    } else {
      // Get the column
      DataItem_ item = this->col_list_->get_item(col);
      DF_Column *column = dynamic_cast<DF_Column *>(item.o);

      // Now get the item in that row for that column
      DF_FloatColumn *float_column = column->as_float();
      return float_column->get(row);
    }
  }

  String *get_string(size_t col, size_t row) {
    // Verify that these values are within bounds
    if (!this->verify_col_row_parameters_(col, row, ColumnType_String)) {
      exit(1);
    } else {
      // Get the column
      DataItem_ item = this->col_list_->get_item(col);
      DF_Column *column = dynamic_cast<DF_Column *>(item.o);

      // Now get the item in that row for that column
      DF_StringColumn *string_column = column->as_string();
      return string_column->get(row);
    }
  }

  /** Set the value at the given column and row to the given value.
   * If the column is not  of the right type or the indices are out of
   * bound, the result is undefined. */
  void set(size_t col, size_t row, int val) {
    // Verify that these values are within bounds
    if (!this->verify_col_row_parameters_(col, row, ColumnType_Integer)) {
      exit(1);
    } else {
      // Get the column
      DataItem_ item = this->col_list_->get_item(col);
      DF_Column *column = dynamic_cast<DF_Column *>(item.o);

      // Now get the item in that row for that column
      DF_IntColumn *int_column = column->as_int();
      int_column->set(row, val);
    }
  }

  void set(size_t col, size_t row, bool val) {
    // Verify that these values are within bounds
    if (!this->verify_col_row_parameters_(col, row, ColumnType_Bool)) {
      exit(1);
    } else {
      // Get the column
      DataItem_ item = this->col_list_->get_item(col);
      DF_Column *column = dynamic_cast<DF_Column *>(item.o);

      // Now get the item in that row for that column
      DF_BoolColumn *bool_column = column->as_bool();
      bool_column->set(row, val);
    }
  }

  void set(size_t col, size_t row, float val) {
    // Verify that these values are within bounds
    if (!this->verify_col_row_parameters_(col, row, ColumnType_Float)) {
      exit(1);
    } else {
      // Get the column
      DataItem_ item = this->col_list_->get_item(col);
      DF_Column *column = dynamic_cast<DF_Column *>(item.o);

      // Now get the item in that row for that column
      DF_FloatColumn *float_column = column->as_float();
      float_column->set(row, val);
    }
  }

  void set(size_t col, size_t row, String *val) {
    // Verify that these values are within bounds
    if (!this->verify_col_row_parameters_(col, row, ColumnType_String)) {
      exit(1);
    } else {
      // Get the column
      DataItem_ item = this->col_list_->get_item(col);
      DF_Column *column = dynamic_cast<DF_Column *>(item.o);

      // Now get the item in that row for that column
      DF_StringColumn *stringColumn = column->as_string();
      stringColumn->set(row, val);
    }
  }

  /** Set the fields of the given row object with values from the columns at
   * the given offset.  If the row is not from the same schema as the
   * dataframe, results are undefined.
   */
  void fill_row(size_t idx, Row &row) {
    // NOTE: This function will add a new row in order to accomodate the
    // provided row index, if the index is out of bounds for this data frame
    // at the time. add_row() will be taking advantage of this functionality.

    // Verify that the row follows the same schema
    if (row.width() != this->schema_->width()) {
      printf("Row provided does not follow the same schema.\n");
      exit(1);
    }
    // Iterate through the row fields and make sure that the column data
    // types match with the schema
    for (size_t i = 0; i < row.width(); i++) {
      if (row.col_type(i) != this->schema_->col_type(i)) {
        printf("Row provided does not follow the same schema.\n");
        exit(1);
      }
    }

    // Iterate down the row to fill the column values at the specified row index
    for (size_t i = 0; i < row.width(); i++) {
      // Get the column at the specified index
      DataItem_ item = this->col_list_->get_item(i);
      DF_Column *column = dynamic_cast<DF_Column *>(item.o);

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

  /** Add a row at the end of this dataframe. The row is expected to have
   *  the right schema and be filled with values, otherwise undedined.  */
  void add_row(Row &row) {
    // Call fill row because it will automatically add rows to the columns in
    // order to accomodate the provided new row
    this->fill_row(this->nrows(), row);
  }

  /** The number of rows in the dataframe. */
  size_t nrows() { return this->num_rows; }

  /** The number of columns in the dataframe.*/
  size_t ncols() { return this->schema_->width(); }

  /**
   * Helper function that creates a row of the contents at the specified
   * 0-indexed row index. This function should only be called by DataFrame
   * and RowerThread_.
   * @param row 0-indexed row index.
   * @return A dynamically allocated row of the contents at the specified
   *         index. The caller will own the return value.
   * @throws Prints an error message and terminates the program if the
   *         specified index is invalid
   */
  Row *create_row_(size_t row) {
    Row *new_row = new Row(*this->schema_);

    // Go through the columns and copy the data into the row
    for (size_t c = 0; c < this->ncols(); c++) {
      // Get the column at the specified index
      DataItem_ item = this->col_list_->get_item(c);
      DF_Column *column = dynamic_cast<DF_Column *>(item.o);

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

  /** Visit rows in order */
  void map(Rower &r) {
    // Iterate down the rows of the dataframe
    for (size_t row = 0; row < this->nrows(); row++) {
      // Create a from the row index
      Row *new_row = this->create_row_(row);

      // Put the row through the rower
      r.accept(*new_row);

      // Delete the row now that we put it through the rower
      delete new_row;
    }
  }

  /** This method clones the Rower and executes the map in parallel. Join is
   * used at the end to merge the results. */
  void pmap(Rower &r) {
    // Get how many cores there are to determine how many threads we can create
    unsigned num_cores = std::thread::hardware_concurrency();

    // Create a pool of threads
    RowerThread_ **threads = new RowerThread_ *[num_cores];
    Rower **child_rowers = new Rower *[num_cores];
    for (size_t id = 0; id < num_cores; id++) {
      child_rowers[id] = dynamic_cast<Rower *>(r.clone());
      threads[id] = new RowerThread_(*this, *child_rowers[id], id, num_cores);
      threads[id]->start();
    }

    // Now wait for all of the threads to finish and then join the children
    // rowers
    for (size_t id = 0; id < num_cores; id++) {
      threads[id]->join();
      r.join_delete(child_rowers[id]);
    }

    // Delete the threads
    delete[] threads;
    delete[] child_rowers;
  }

  /** Create a new dataframe, constructed from rows for which the given Rower
   * returned true from its accept method. */
  DataFrame *filter(Rower &r) {
    // Create a new dataframe with the same schema
    DataFrame *ret_value = new DataFrame(*this->schema_);

    // Iterate down the rows
    for (size_t row = 0; row < this->nrows(); row++) {
      // Create a from the row index
      Row *new_row = this->create_row_(row);

      // Put the row through the rower
      if (r.accept(*new_row)) {
        // The rower returns true. That means that this row can be added to
        // the new dataframe
        ret_value->add_row(*new_row);
      }

      // Delete the row now that we put it through the rower
      delete new_row;
    }

    return ret_value;
  }

  /** Print the dataframe in SoR format to standard output. */
  void print() {
    // Use a rower to print out the dataframe
    PrintRower rower;
    this->map(rower);
  }

  /**
   * Generates a Dataframe out of the provided array, and then stores it
   * within the map at the specified key.
   * @param key The key associated with the new dataframe.
   * @param map The map that will store the dataframe at the specified key.
   * @param num_values The number of values of the provided array.
   * @param values The array of valuas that will be used to create a dataframe
   * @return The newly created dataframe
   */
  static DataFrame *fromArray(Key *key, Map *map, size_t num_values,
      float *values) {
    assert(key != nullptr);
    assert(map != nullptr);
    assert(values != nullptr);

    // Create the dataframe
    Schema schema("F");
    DataFrame *ret_value = new DataFrame(schema);

    for (size_t i = 0; i < num_values; i++) {
      Row r(ret_value->get_schema());
      r.set(0, values[i]);
      ret_value->add_row(r);
    }

    map->put(key, ret_value);

    return ret_value;
  }

  static DataFrame *fromArray(Key *key, Map *map, size_t num_values,
                              int *values) {
    assert(key != nullptr);
    assert(map != nullptr);
    assert(values != nullptr);

    // Create the dataframe
    Schema schema("I");
    DataFrame *ret_value = new DataFrame(schema);

    for (size_t i = 0; i < num_values; i++) {
      Row r(ret_value->get_schema());
      r.set(0, values[i]);
      ret_value->add_row(r);
    }

    map->put(key, ret_value);

    return ret_value;
  }

  static DataFrame *fromArray(Key *key, Map *map, size_t num_values,
                              bool *values) {
    assert(key != nullptr);
    assert(map != nullptr);
    assert(values != nullptr);

    // Create the dataframe
    Schema schema("B");
    DataFrame *ret_value = new DataFrame(schema);

    for (size_t i = 0; i < num_values; i++) {
      Row r(ret_value->get_schema());
      r.set(0, values[i]);
      ret_value->add_row(r);
    }

    map->put(key, ret_value);

    return ret_value;
  }

  static DataFrame *fromArray(Key *key, Map *map, size_t num_values,
                              String **values) {
    assert(key != nullptr);
    assert(map != nullptr);
    assert(values != nullptr);

    // Create the dataframe
    Schema schema("S");
    DataFrame *ret_value = new DataFrame(schema);

    for (size_t i = 0; i < num_values; i++) {
      Row r(ret_value->get_schema());
      r.set(0, values[i]);
      ret_value->add_row(r);
    }

    map->put(key, ret_value);

    return ret_value;
  }
};

RowerThread_::RowerThread_(DataFrame &df, Rower &rower, size_t id,
    unsigned inc) {
  this->df_ = &df;
  this->rower_ = &rower;
  this->id_ = id;
  this->inc_ = inc;
}

/**
 * Overrides the run function of Thread class.
 */
void RowerThread_::run() {
  for (size_t r = this->id_; r < this->df_->nrows(); r += this->inc_) {
    // Create a from the row index
    Row *new_row = this->df_->create_row_(r);

    // Put the row through the rower
    this->rower_->accept(*new_row);

    // Delete the row now that we put it through the rower
    delete new_row;
  }
}

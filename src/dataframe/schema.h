/**
 * Name: Snowy Chen, Joe Song
 * Date: 14 February 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

//lang::CwC

#pragma once

#include <stdlib.h>

#include "object.h"
#include "custom_string.h"

/**
 * A helper class object in order to store column and its name. This is a
 * pair value exclusively used by Schema.
 */
class ColumnInfo_ : public Object {
public:
  ColumnType_t type_;

  /**
   * Constructs a basic object class that contains information about a
   * column.
   * @param type The data type of the column stored.
   */
  explicit ColumnInfo_(ColumnType_t type) {
    this->type_ = type;
  }
};

/*************************************************************************
 * Schema::
 * A schema is a description of the contents of a data frame, the schema
 * knows the number of columns and number of rows, the type of each column,
 * optionally columns and rows can be named by strings.
 * The valid types are represented by the chars 'S', 'B', 'I' and 'F'.
 */
class Schema : public Object {
public:
  // ASSERT: All items within this list is of type ColumnInfo_.
  LinkedListArray_ *column_list_;

  /** Copying constructor */
  Schema(Schema &from) : Schema() {
    // Copy the information about the columns into this schema's column list
    LinkedListArrayIter_ *iter = nullptr;
    for (iter = new LinkedListArrayIter_(*from.column_list_);
      iter->has_next();
      /* Incrementer is built into the for loop */) {

      // Cast the next item in the column list to a ColumnWithName to get the
      // data.
      DataItem_ item = iter->get_next();
      ColumnInfo_ *col_type_name = dynamic_cast<ColumnInfo_ *>(item.o);

      // Now clone the ColumnNameType_ to be referenced to this schema
      this->add_column(col_type_name->type_);
    }
  }

  /** Create an empty schema **/
  Schema() {
    this->column_list_ = new LinkedListArray_();
  }

  /** Create a schema from a string of types. A string that contains
   * characters other than those identifying the four type results in
   * undefined behavior. The argument is external, a nullptr argument is
   * undefined. **/
  Schema(const char *types) : Schema() {
    if (types == nullptr) {
      printf("Cannot interpret provided schema data type arguments.\n");
      exit(1);
    } else {
      // Iterate through the characters within the types until we reach the
      // null terminator.
      for (int i = 0; types[i] != '\0'; i++) {
        this->add_column(types[i]);
      }
    }
  }

  /**
   * Deconstruct the schema.
   */
  ~Schema() override {
    // Iterate through the column list in order to destroy the columns data
    LinkedListArrayIter_ *iter = nullptr;
    for (iter = new LinkedListArrayIter_(*this->column_list_); iter->has_next();
         /* Incrementer is built into the for loop */) {
      DataItem_ item = iter->get_next();

      // Cast the item to a ColumnWithName in order to destroy
      ColumnInfo_ *col_type_name = dynamic_cast<ColumnInfo_ *>(item.o);

      // Destroy the objects that were allocated internally to the schema.
      delete col_type_name;
    }

    // Now delete the lists themselves.
    delete this->column_list_;
  }

  /**
   * Adds a column of the given type to the schema.
   * @param typ
   */
  void add_column(char typ) {
    // Get the desired column type and create a new column to add to the
    // list
    if ((typ != static_cast<char>(ColumnType_Bool))
      &&  (typ != static_cast<char>(ColumnType_Integer))
          &&  (typ != static_cast<char>(ColumnType_Float))
              &&  (typ != static_cast<char>(ColumnType_String))) {
      printf("Unrecognized column data type %c.\n", typ);
      exit(1);
    }
    else {
      // Valid column type
      ColumnInfo_ *new_col_name =
          new ColumnInfo_(static_cast<ColumnType_t>(typ));
      DataItem_ data_item;
      data_item.o = new_col_name;
      this->column_list_->add_new_item(data_item);
    }
  }

  /** Return type of column at idx. An idx >= width is undefined. */
  char col_type(size_t idx) {
    DataItem_ col_name_item = this->column_list_->get_item(idx);

    // Refer the returned item as an object ColumnNameType_, because we
    // already know that all items added to this list are always going to be
    // ColumnNameType_
    ColumnInfo_ *col_name =
        dynamic_cast<ColumnInfo_ *>(col_name_item.o);
    return col_name->type_;
  }

  /** The number of columns */
  size_t width() { return this->column_list_->size(); }
};

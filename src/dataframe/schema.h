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
class ColumnNameType_ : public Object {
public:
  String *name_;
  ColumnType_t type_;

  /**
   * Constructs a basic object class that contains information about a
   * column, namely the name and the column type.
   * @param type The data type of the column stored.
   * @param name A pointer to the column name. This structure will not own
   *        the name. The name is also optional, so it can be given a
   *        parameter of nullptr.
   */
  ColumnNameType_(ColumnType_t type, String *name) {
    this->type_ = type;
    this->name_ = name;
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
  // ASSERT: All items within this list is of type ColumnNameType_.
  LinkedListArray_ *column_list_;

  // ASSERT: All items within this list is of type String.
  LinkedListArray_ *row_name_list_;

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
      ColumnNameType_ *col_type_name = dynamic_cast<ColumnNameType_ *>(item.o);

      // Now clone the ColumnNameType_ to be referenced to this schema
      this->add_column(col_type_name->type_, col_type_name->name_);
    }
    this->row_name_list_ = new LinkedListArray_(*from.row_name_list_);
  }

  /** Create an empty schema **/
  Schema() {
    this->column_list_ = new LinkedListArray_();
    this->row_name_list_ = new LinkedListArray_();
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
        this->add_column(types[i], nullptr);
      }
    }
  }

  /**
   * Deconstruct the schema.
   */
  ~Schema() {
    // Iterate through the column list in order to destroy the columns data
    LinkedListArrayIter_ *iter = nullptr;
    for (iter = new LinkedListArrayIter_(*this->column_list_); iter->has_next();
         /* Incrementer is built into the for loop */) {
      DataItem_ item = iter->get_next();

      // Cast the item to a ColumnWithName in order to destroy
      ColumnNameType_ *col_type_name = dynamic_cast<ColumnNameType_ *>(item.o);

      // Destroy the objects that were allocated internally to the schema.
      delete col_type_name;
    }

    // Now delete the lists themselves.
    delete this->column_list_;
    delete this->row_name_list_;
  }

  /** Add a column of the given type and name (can be nullptr), name
   * is external. */
  void add_column(char typ, String *name) {
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
      ColumnNameType_ *new_col_name = new ColumnNameType_
          (static_cast<ColumnType_t>(typ),
           name);
      DataItem_ data_item;
      data_item.o = new_col_name;
      this->column_list_->add_new_item(data_item);
    }
  }

  /** Add a row with a name (possibly nullptr), name is external. */
  void add_row(String *name) {
    DataItem_ data_item;
    data_item.s = name;
    this->row_name_list_->add_new_item(data_item);
  }

  /** Return name of row at idx; nullptr indicates no name. An idx >= width
   * is undefined. */
  String *row_name(size_t idx) {
    DataItem_ row_name_item = this->row_name_list_->get_item(idx);

    // Refer the returned item as a string because we already know that all
    // items added to this list are always going to be a string.
    return row_name_item.s;
  }

  /** Return name of column at idx; nullptr indicates no name given.
   *  An idx >= width is undefined.*/
  String *col_name(size_t idx) {
    DataItem_ col_name_item = this->column_list_->get_item(idx);

    // Refer the returned item as an object ColumnNameType_, because we
    // already know that all items added to this list are always going to be
    // ColumnNameType_
    ColumnNameType_ *col_name =
        dynamic_cast<ColumnNameType_ *>(col_name_item.o);
    return col_name->name_;
  }

  /** Return type of column at idx. An idx >= width is undefined. */
  char col_type(size_t idx) {
    DataItem_ col_name_item = this->column_list_->get_item(idx);

    // Refer the returned item as an object ColumnNameType_, because we
    // already know that all items added to this list are always going to be
    // ColumnNameType_
    ColumnNameType_ *col_name =
        dynamic_cast<ColumnNameType_ *>(col_name_item.o);
    return col_name->type_;
  }

  /** Given a column name return its index, or -1. */
  int col_idx(const char *name) {
    // Make sure that the name is not null
    if (name == nullptr) {
      return -1;
    }

    String *desired_name = new String(name);

    // Iterate through the row names until the desired one gets found
    int index = 0;
    LinkedListArrayIter_ *iter = nullptr;
    for (iter = new LinkedListArrayIter_(*this->column_list_); iter->has_next();
         /* Incrementer is built into the for loop */) {
      DataItem_ item = iter->get_next();

      // Cast the item to a ColumnWithName in order to get the name
      ColumnNameType_ *col_name = dynamic_cast<ColumnNameType_ *>(item.o);
      if (desired_name->equals(col_name->name_)) {
        // Found the name!

        // Deallocate the memory as appropriate
        delete iter;
        delete desired_name;

        return index;
      }

      // Increment the index as we move onto the next one
      index += 1;
    }

    // Deallocate the memory as appropriate
    delete iter;
    delete desired_name;

    return -1;
  }

  /** Given a row name return its index, or -1. */
  int row_idx(const char *name) {
    // Make sure that the name is not null
    if (name == nullptr) {
      return -1;
    }

    String *desired_name = new String(name);

    // Iterate through the row names until the desired one gets found
    int index = 0;
    LinkedListArrayIter_ *iter = nullptr;
    for (iter = new LinkedListArrayIter_(*this->row_name_list_);
         iter->has_next();
         /* Incrementer is built into the for loop */) {
      DataItem_ item = iter->get_next();
      if (desired_name->equals(item.s)) {
        // Found the name!
        // Deallocate the memory as appropriate
        delete iter;
        delete desired_name;

        return index;
      }

      // Increment the index as we move onto the next one
      index += 1;
    }

    // Deallocate the memory as appropriate
    delete iter;
    delete desired_name;

    return -1;
  }

  /** The number of columns */
  size_t width() { return this->column_list_->size(); }

  /** The number of rows */
  size_t length() { return this->row_name_list_->size(); }
};

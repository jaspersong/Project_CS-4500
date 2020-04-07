/**
 * Name: Snowy Chen, Joe Song
 * Date: 21 March 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::Cpp

#include "schema.h"
#include "dataframe_column.h"

/**
 * A helper class object in order to store column and its name. This is a
 * pair value exclusively used by Schema.
 */
class ColumnInfo_ : public CustomObject {
public:
  ColumnType_t type_;

  /**
   * Constructs a basic object class that contains information about a
   * column.
   * @param type The data type of the column stored.
   */
  explicit ColumnInfo_(ColumnType_t type) { this->type_ = type; }
};

Schema::Schema(Schema &from) : Schema() {
  // Copy the information about the columns into this schema's column list
  for (size_t i = 0; i < from.column_list->size(); i++) {
    // Cast the next item in the column list to a ColumnWithName to get the
    // data.
    DataItem_ item = from.column_list->get_item(i);
    ColumnInfo_ *col_type_name = dynamic_cast<ColumnInfo_ *>(item.o);

    // Now clone the ColumnNameType_ to be referenced to this schema
    this->add_column(col_type_name->type_);
  }
}

Schema::Schema() { this->column_list = new ArrayOfArrays(); }

Schema::Schema(const char *types) : Schema() {
  if (types == nullptr) {
    printf("Cannot interpret provided schema data type arguments.\n");
    assert(false);
  } else {
    // Iterate through the characters within the types until we reach the
    // null terminator.
    for (int i = 0; types[i] != '\0'; i++) {
      this->add_column(types[i]);
    }
  }
}

Schema::~Schema() {
  // Iterate through the column list in order to destroy the columns data
  for (size_t i = 0; i < this->column_list->size(); i++) {
    DataItem_ item = this->column_list->get_item(i);

    // Cast the item to a ColumnWithName in order to destroy
    auto *col_type_name = dynamic_cast<ColumnInfo_ *>(item.o);

    // Destroy the objects that were allocated internally to the schema.
    delete col_type_name;
  }

  // Now delete the lists themselves.
  delete this->column_list;
}

void Schema::add_column(char typ) {
  // Get the desired column type and create a new column to add to the
  // list
  if ((typ != static_cast<char>(ColumnType_Bool)) &&
      (typ != static_cast<char>(ColumnType_Integer)) &&
      (typ != static_cast<char>(ColumnType_Float)) &&
      (typ != static_cast<char>(ColumnType_String))) {
    printf("Unrecognized column data type %c.\n", typ);
    assert(false);
  } else {
    // Valid column type
    ColumnInfo_ *new_col_name = new ColumnInfo_(static_cast<ColumnType_t>(typ));
    DataItem_ data_item;
    data_item.o = new_col_name;
    this->column_list->add_new_item(data_item);
  }
}

char Schema::col_type(size_t idx) {
  DataItem_ col_name_item = this->column_list->get_item(idx);

  // Refer the returned item as an object ColumnNameType_, because we
  // already know that all items added to this list are always going to be
  // ColumnNameType_
  auto *col_name = dynamic_cast<ColumnInfo_ *>(col_name_item.o);
  return col_name->type_;
}

size_t Schema::width() { return this->column_list->size(); }

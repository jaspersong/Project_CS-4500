/**
 * Name: Snowy Chen, Joe Song
 * Date: 14 February 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// lang::CwC
#include <stdio.h>
#include <stdlib.h>

#include "dataframe.h"
#include "parser.h"

/**
 * A helper class that integrates the 4500ne team's Sorer implementation into
 * a Dataframe to be used within this project.
 */
class SorerIntegrator {
public:
  FILE *file_;
  size_t start_;
  size_t len_;
  size_t file_size_;

  ColumnSet *set_;
  SorParser *parser_;

  /**
   * Constructs a Sorer integrator.
   * @param file_path The open sorer file that will be read in.
   * @param start The starting byte of the file to start reading
   * @param len The number of bytes to read in from the file
   * @throws Terminates the program if the provided file cannot be opened.
   */
  SorerIntegrator(const char *file_path, size_t start, size_t len) {
    assert(file_path != nullptr);

    // Open the file
    this->file_ = fopen(file_path, "r");
    assert(this->file_ != nullptr);

    this->start_ = start;
    this->len_ = len;

    fseek(this->file_, 0, SEEK_END);
    this->file_size_ = ftell(this->file_);
    fseek(this->file_, 0, SEEK_SET);

    this->set_ = nullptr;
    this->parser_ = nullptr;
  }

  /**
   * Constructs a Sorer integrator.
   * @param file_path The file path to the Sorer file
   * @param start The starting byte to start reading from.
   */
  SorerIntegrator(const char *file_path, size_t start)
      : SorerIntegrator(file_path, start, -1) {
    this->len_ = this->file_size_;
  }

  /**
   * Constructs a Sorer integrator.
   * @param file_path The file path to the Sorer file
   */
  explicit SorerIntegrator(const char *file_path)
      : SorerIntegrator(file_path, 0) {}

  /**
   * Deconstructs the sorer integrator
   */
  ~SorerIntegrator() {
    fclose(this->file_);
    delete this->parser_;
  }

  /**
   * Parses the Sorer file, but does not convert it into a DataFrame.
   * This function should be called before
   */
  void parse() {
    // No need to parse if this was already parsed
    if (this->set_ == nullptr) {
      this->parser_ =
          new SorParser(this->file_, this->start_, this->start_ + this->len_,
                        this->file_size_);
      this->parser_->guessSchema();
      this->parser_->parseFile();
      this->set_ = this->parser_->getColumnSet();
    }
  }

  /**
   * Converts the parsed data from the Sorer into a DataFrame.
   * @return The dataframe representing the sorer file. The memory is
   *        dynamically allocated and will now be owned by the caller.
   * @throws Throws an error if parse() was not called, and terminates the
   *        program.
   */
  DataFrame *convert() {
    // Create the schema from the columns
    Schema schema;
    for (size_t i = 0; i < this->set_->getLength(); i++) {
      ColumnType type = this->set_->getColumn(i)->getType();
      switch (type) {
      case ColumnType::STRING:
        schema.add_column('S');
        break;
      case ColumnType::INTEGER:
        schema.add_column('I');
        break;
      case ColumnType::FLOAT:
        schema.add_column('F');
        break;
      case ColumnType::BOOL:
        schema.add_column('B');
        break;
      case ColumnType::UNKNOWN:
      default:
        // Unrecognized column type. Do nothing and skip it.
        break;
      }
    }

    // Now create a dataframe
    DataFrame *ret_value = new DataFrame(schema);

    // Copy the values from the sorer to the dataframe
    for (size_t r = 0; r < this->set_->getColumn(0)->getLength(); r++) {
      Row row(ret_value->get_schema());
      row.set_idx(r);

      for (size_t c = 0; c < this->set_->getLength(); c++) {
        BaseColumn *col = this->set_->getColumn(c);
        ColumnType type = col->getType();
        switch (type) {
        case ColumnType::STRING: {
          StringColumn *str_col = (StringColumn *)col;
          if (str_col->isEntryPresent(r)) {
            row.set(c, new String(str_col->getEntry(r)));
          } else {
            row.set(c, nullptr);
          }
          break;
        }
        case ColumnType::INTEGER: {
          IntegerColumn *int_col = (IntegerColumn *)col;
          if (int_col->isEntryPresent(r)) {
            row.set(c, int_col->getEntry(r));
          } else {
            row.set(c, (int)0);
          }
          break;
        }
        case ColumnType::FLOAT: {
          FloatColumn *float_col = (FloatColumn *)col;
          if (float_col->isEntryPresent(r)) {
            row.set(c, float_col->getEntry(r));
          } else {
            row.set(c, 0.0f);
          }
          break;
        }
        case ColumnType::BOOL: {
          BoolColumn *bool_col = (BoolColumn *)col;
          if (bool_col->isEntryPresent(r)) {
            row.set(c, bool_col->getEntry(r));
          } else {
            row.set(c, false);
          }
          break;
        }
        case ColumnType::UNKNOWN:
        default:
          // Unrecognized column type. Do nothing and skip it.
          break;
        }
      }

      ret_value->add_row(row);
    }

    return ret_value;
  }
};

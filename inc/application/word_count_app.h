/**
 * Name: Snowy Chen, Joe Song
 * Date: 1 April 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::Cpp

#pragma once

#include "application.h"
#include "application_network_interface.h"
#include "key.h"

/**
 * A map where the key is a string, and the value is an integer.
 */
class SIMap : public CustomObject {
public:
  ~SIMap() override;

  bool contains(String &key);
  int get(String &key);
  void set(String &key, int value);

  size_t size();

  void start_iteration();
  bool has_next();
  void next_iter();
  String *get_iter_key();
  int get_iter_value();

  void print_vals();

private:
  class StringComp {
  public:
    bool operator()(const String *lhs, const String *rhs) const {
      return strcmp(lhs->c_str(), rhs->c_str()) < 0;
    }
  };

  std::map<String *, int, StringComp> map;
  std::map<String *, int, StringComp>::iterator iter = map.end();
};

/**
 * FileReader pulled from Jan Vitek's assignment page.
 */
class FileReader : public Writer {
public:
  static const size_t BUFSIZE = 1024;

  /** Creates the reader and opens the file for reading.  */
  explicit FileReader(String *file_name);
  ~FileReader() override;

  /** Reads next word and stores it in the row. Actually read the word.
      While reading the word, we may have to re-fill the buffer  */
  void visit(Row &r) override;

  /** Returns true when there are no more words to read.  There is nothing
     more to read if we are at the end of the buffer and the file has
     all been read.     */
  bool done() override;

private:
  char buf_[BUFSIZE + 1] = {0};
  size_t end_ = 0;
  size_t i_ = 0;
  FILE *file_;

  /** Reads more data from the file. */
  void fillBuffer_();

  /** Skips spaces.  Note that this may need to fill the buffer if the
      last character of the buffer is space itself.  */
  void skipWhitespace_();
};

/**
 * A reader that runs through a dataframe consisting of a single column of
 * string words, and counts the number of times each word appears within the
 * dataframe and stores them into the provided SI Map
 */
class DFWordCounter : public Reader {
public:
  explicit DFWordCounter(SIMap *dest_map);
  bool accept(Row &r) override;

private:
  SIMap *map;
};

/**
 * A writer that creates a dataframe from an SIMap.
 */
class SIMapToDF : public Writer {
public:
  explicit SIMapToDF(SIMap *map);

  void visit(Row &r) override;
  bool done() override;

private:
  SIMap *map;
};

/**
 * A reader that reads through multiple dataframes and merges their contents
 * together. Their contents consist of a string and a value, where the string
 * is the key, and the values of that key is added up together across the
 * multiple dataframes. Then the counts are stored within the map.
 */
class WordCountMerger : public Reader {
public:
  explicit WordCountMerger(SIMap *dest_map);
  bool accept(Row &r) override;

private:
  SIMap *map;
};

/****************************************************************************
 * Calculate a word count for given file:
 *   1) read the data (single node)
 *   2) produce word counts per homed chunks, in parallel
 *   3) combine the results
 ****************************************************************************/
class WordCount : public Application {
public:
  // TODO: 8 and above nodes causes problems with sending directory messages
  //  from the server
  static const size_t NUM_NODES = 3;
  Key txt = Key("txt");

  explicit WordCount(const char *file_name);
  ~WordCount() override;
  void main() override;

private:
  String file_name;
  Key *wordcount_keys[NUM_NODES] = {};

  void local_count();
  void merge_counts();
};

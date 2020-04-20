/**
 * Name: Snowy Chen, Joe Song
 * Date: 1 April 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::Cpp

#include "word_count_app.h"

SIMap::~SIMap() {
  for (this->start_iteration(); this->has_next(); this->next_iter()) {
    delete this->get_iter_key();
  }
}

bool SIMap::contains(String &key) {
  std::map<String *, int, StringComp>::iterator it;
  it = this->map.find(&key);
  return it != this->map.end();
}

int SIMap::get(String &key) {
  return this->map[&key];
}

void SIMap::set(String &key, int value) {
  if (this->contains(key)) {
    // This is to replace a value of a pre-existing key. Just set the value
    this->map[&key] = value;
  } else {
    // this is a brand new key. Create an instance and set the value accordingly
    auto *new_key = new String(key);
    this->map[new_key] = value;
  }
}

void SIMap::start_iteration() {
  this->iter = this->map.begin();
}

bool SIMap::has_next() {
  return this->iter != this->map.end();
}

void SIMap::next_iter() {
  this->iter++;
}

String *SIMap::get_iter_key() {
  return this->iter->first;
}

int SIMap::get_iter_value() {
  return this->iter->second;
}

void SIMap::print_vals() {
  for (auto const& kv : this->map) {
    printf("%s: %d\n", kv.first->c_str(), kv.second);
  }
}
size_t SIMap::size() {
  return this->map.size();
}

/****************************************************************************/

FileReader::FileReader(String *file_name) {
  file_ = fopen(file_name->c_str(), "r");
  assert(this->file_);

  fillBuffer_();
  skipWhitespace_();
}

FileReader::~FileReader() {
  fclose(this->file_);
}

void FileReader::visit(Row &r) {
  assert(i_ < end_);
  assert(!isspace(buf_[i_]));
  size_t wStart = i_;
  while (true) {
    if (i_ == end_) {
      if (feof(file_)) {
        ++i_;
        break;
      }
      i_ = wStart;
      wStart = 0;
      fillBuffer_();
    }
    if (isspace(buf_[i_]))
      break;
    ++i_;
  }
  buf_[i_] = 0;
  String word(buf_ + wStart, i_ - wStart);
  r.set(0, &word);
  ++i_;
  skipWhitespace_();
}

bool FileReader::done() { return (i_ >= end_) && feof(file_); }

void FileReader::fillBuffer_() {
  size_t start = 0;
  // compact unprocessed stream
  if (i_ != end_) {
    start = end_ - i_;
    memcpy(buf_, buf_ + i_, start);
  }
  // read more contents
  end_ = start + fread(buf_ + start, sizeof(char), BUFSIZE - start, file_);
  i_ = start;
}

void FileReader::skipWhitespace_() {
  while (true) {
    if (i_ == end_) {
      if (feof(file_))
        return;
      fillBuffer_();
    }
    // if the current character is not whitespace, we are done
    if (!isspace(buf_[i_]))
      return;
    // otherwise skip it
    ++i_;
  }
}

/****************************************************************************/

DFWordCounter::DFWordCounter(SIMap *dest_map) {
  assert(dest_map != nullptr);
  this->map = dest_map;
}

bool DFWordCounter::accept(Row &r) {
  String *word = r.get_string(0);
  assert(word != nullptr);

  // Increment the word count
  int num = 0;
  if (this->map->contains(*word)) {
    num = this->map->get(*word);
  }
  num++;

  // Add it to the map
  this->map->set(*word, num);

  return true;
}

/****************************************************************************/

SIMapToDF::SIMapToDF(SIMap *map) {
  assert(map);
  this->map = map;
  this->map->start_iteration();
}

void SIMapToDF::visit(Row& r) {
  r.set(0, this->map->get_iter_key());
  r.set(1, this->map->get_iter_value());
  this->map->next_iter();
}

bool SIMapToDF::done() {
  return !this->map->has_next();
}

/****************************************************************************/

WordCountMerger::WordCountMerger(SIMap *dest_map) {
  assert(dest_map != nullptr);
  this->map = dest_map;
}

bool WordCountMerger::accept(Row &r) {
  String *word = r.get_string(0);
  int count = r.get_int(1);
  assert(word != nullptr);

  // Add this count to the map
  if (this->map->contains(*word)) {
    // Add the count to the current count value for the word, if applicable
    count += this->map->get(*word);
  }
  this->map->set(*word, count);

  return true;
}

/****************************************************************************/

WordCount::WordCount(const char *file_name) :
  Application(WordCount::NUM_NODES), file_name(file_name) {
  // Dynamically create the keys containing the final word count dataframe
  for (size_t i = 0; i < WordCount::NUM_NODES; i++) {
    String * key_name = StrBuff().c("wc-").c(i).get();
    this->wordcount_keys[i] = new Key(key_name->c_str());
    delete key_name;
  }
}

WordCount::~WordCount() {
  for (auto wordcount_key : this->wordcount_keys) {
    delete wordcount_key;
  }
}

void WordCount::main() {
  if (this->get_node_id() == 0) {
    // Create the dataframe from the text file
    FileReader fr(&this->file_name);
    KeyValueStore::from_visitor(this->txt, this->kv, "S", fr);
  }

  // Now count the values
  this->local_count();

  if (this->get_node_id() == 0) {
    // Now merge in all of the word counts and print it out
    this->merge_counts();
  }
}

void WordCount::local_count() {
  SIMap word_count;
  DFWordCounter word_counter(&word_count);

  // Iterate through all of the local dataframes to get the wordcount
  printf("Node %zu: starting local count...\n", this->kv->get_home_id());
  DistributedValue *value = this->kv->wait_and_get(this->txt);
  value->local_map(word_counter);

  // Now transform the word counter into a dataframe associated with this
  // node's word count key
  SIMapToDF map_to_df(&word_count);
  String *key_name = this->wordcount_keys[this->kv->get_home_id()]->get_name();
  printf("Storing count to key %s\n", key_name->c_str());
  KeyValueStore::from_visitor(*this->wordcount_keys[this->kv->get_home_id()],
      this->kv, "SI", map_to_df);
}

void WordCount::merge_counts() {
  SIMap total_word_count;
  WordCountMerger merger(&total_word_count);

  // Get all of the local word counts from the other nodes and then sum them
  // all up into the total word count
  for (auto wordcount_key : this->wordcount_keys) {
    DistributedValue *df = this->kv->wait_and_get(*wordcount_key);
    df->map(merger);
  }

  // Now print out the word counter
  printf("Different words: %zu\n", total_word_count.size());
  total_word_count.print_vals();
  printf("Completed word count.\n");
}

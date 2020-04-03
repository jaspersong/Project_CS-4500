/**
 * Name: Snowy Chen, Joe Song
 * Date: 1 April 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::Cpp

#include "word_count_app.h"

Sys helper;
const char *expected_signal_message = "distribution done";

bool SIMap::contains(String &key) {
  std::map<String *, int, StringComp>::iterator it;
  it = this->map.find(&key);
  return it != this->map.end();
}

int SIMap::get(String &key) {
  return this->map[&key];
}

void SIMap::set(String &key, int value) {
  this->map[&key] = value;
}

void SIMap::start_iteration() {
  this->iter = this->map.begin();
}

bool SIMap::has_next() {
  return this->iter != this->map.end();
}

void SIMap::next_iter() {
  std::next(this->iter);
}

String *SIMap::get_iter_key() {
  return this->iter->first;
}

int SIMap::get_iter_value() {
  return this->iter->second;
}

void SIMap::print_vals() {
  for (auto const& kv : this->map) {
    helper.p(kv.first->c_str()).p(": ").pln(kv.second);
  }
}
size_t SIMap::size() {
  return this->map.size();
}

/****************************************************************************/

FileReader::FileReader(String *file_name) {
  file_ = fopen(file_name->c_str(), "r");
  if (file_ == nullptr) {
    helper.p("Cannot open file ").pln(file_name->c_str());
  }

  buf_ = new char[BUFSIZE + 1]; //  null terminator
  fillBuffer_();
  skipWhitespace_();
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

SIMapToDF::SIMapToDF(SIMap& map) : map_(map) {
  this->map_.start_iteration();
}

void SIMapToDF::next() {
  this->map_.next_iter();
}

void SIMapToDF::visit(Row& r) {
  r.set(0, this->map_.get_iter_key());
  r.set(1, this->map_.get_iter_value());
  this->map_.next_iter();
}

bool SIMapToDF::done() {
  return this->map_.has_next();
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

WordCountStatusHandler::WordCountStatusHandler(Lock *distro_complete_signal) {
  assert(distro_complete_signal);
  this->signal = distro_complete_signal;
}

bool WordCountStatusHandler::handle_status(Status *msg) {
  String *message = msg->get_message();
  String expecterd_msg(expected_signal_message);

  if (expecterd_msg.equals(message)) {
    // We got the expected message stating that the distribution of the
    // dataframe has been completed. Flip on the signal to start the local
    // counting
    this->signal->notify_all();
  }

  delete message;
  return false;
}

/****************************************************************************/

WordCount::WordCount() : Application(WordCount::NUM_NODES) {
  // Dynamically create the keys containing the final word count dataframe
  for (size_t i = 0; i < WordCount::NUM_NODES; i++) {
    char key_name[16];
    snprintf(key_name, 16, "wc-%zu", i);
    this->wordcount_keys[i] = new Key(key_name, i);
  }

  this->status_handler = new WordCountStatusHandler(&this->distribution_signal);
}

WordCount::~WordCount() {
  delete this->status_handler;
  for (auto wordcount_key : this->wordcount_keys) {
    delete wordcount_key;
  }
}

void WordCount::main() {
  if (this->get_node_id() == 0) {
    // Create the dataframe from the text file
    String file_name("../data/shakespeare.txt");
    FileReader fr(&file_name);
    KeyValueStore::from_visitor("txt-part-", this->kv, "S", fr, 50);

    // Now notify that the dataframe has been distributed
    String distro_done_msg(expected_signal_message);
    for (size_t i = 1; i < NUM_NODES; i++) {
      this->kv->send_status_message(i, distro_done_msg);
    }
  }
  else {
    // Wait for the producer to distribute the dataframes to each of the
    // nodes before counting the values.
    this->distribution_signal.wait();
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
  helper.p("Node ").p(this->kv->get_home_id()).pln(": starting local count...");
  for (this->kv->start_iter(); this->kv->has_next(); this->kv->next_iter()) {
    this->kv->get_iter_value()->map(word_counter);
  }

  // Now transform the word counter into a dataframe associated with this
  // node's word count key
  SIMapToDF map_to_df(word_count);
  helper.p("Storing count to key ").pln(
      *this->wordcount_keys[this->kv->get_home_id()]->c_str());
  KeyValueStore::from_visitor(*this->wordcount_keys[this->kv->get_home_id()],
      this->kv, "SI", map_to_df);
}

void WordCount::merge_counts() {
  SIMap total_word_count;
  WordCountMerger merger(&total_word_count);

  // Get all of the local word counts from the other nodes and then sum them
  // all up into the total word count
  for (auto wordcount_key : this->wordcount_keys) {
    DataFrame *df = this->kv->wait_and_get(*wordcount_key);
    df->map(merger);
    delete df;
  }

  // Now print out the word counter
  helper.p("Different words: ").pln(total_word_count.size());
  total_word_count.print_vals();
}

/**
 * Name: Snowy Chen, Joe Song
 * Date: 22 March 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::Cpp

#include "custom_string.h"

String::String(char const *cstr, size_t len) {
  size_ = len;
  cstr_ = new char[size_ + 1];
  memcpy(cstr_, cstr, size_ + 1);
  cstr_[size_] = 0; // terminate
}

String::String(bool steal, char *cstr, size_t len) {
  assert(steal && cstr[len] == 0);
  size_ = len;
  cstr_ = cstr;
}

String::String(char const *cstr) : String(cstr, strlen(cstr)) {}

String::String(String &from) : CustomObject(from) {
  size_ = from.size_;
  cstr_ = new char[size_ + 1]; // ensure that we copy the terminator
  memcpy(cstr_, from.cstr_, size_ + 1);
}

String::~String() { delete[] cstr_; }

char String::at(size_t index) {
  assert(index < size_);
  return cstr_[index];
}

bool String::equals(CustomObject *other) const {
  if (other == this)
    return true;
  auto *x = dynamic_cast<String *>(other);
  if (x == nullptr)
    return false;
  if (size_ != x->size_)
    return false;
  return strncmp(cstr_, x->cstr_, size_) == 0;
}

char *String::steal() {
  char *res = cstr_;
  cstr_ = nullptr;
  return res;
}

size_t String::hash_me() {
  size_t hash = 0;
  for (size_t i = 0; i < size_; ++i)
    hash = cstr_[i] + (hash << 6) + (hash << 16) - hash;
  return hash;
}

size_t String::serialization_required_bytes() {
  return Serializer::get_required_bytes(this->size_) + this->size_ + 1;
}

void String::serialize(Serializer &serializer) {
  // Set the size of the string
  serializer.set_size_t(this->size_);

  // Now copy the string to the serializer, including the null terminator
  serializer.set_generic(reinterpret_cast<unsigned char *>(this->cstr_),
                         this->size_ + 1);
}

String *String::deserialize_as_string(Deserializer &deserializer) {
  String *ret_value = nullptr;

  if (deserializer.has_size_t()) {
    // Get the expected string length
    size_t length = deserializer.get_size_t();

    // Verify that there is enough space left in the buffer to contain a
    // string of that length
    if (deserializer.get_num_bytes_left() >= length + 1) {
      // Build the string by grabbing the bytes in from the deserializer
      char *str = new char[length + 1];
      for (size_t i = 0; i < length + 1; i++) {
        str[i] = static_cast<char>(deserializer.get_byte());
      }

      ret_value = new String(true, str, length);
    }
  }

  return ret_value;
}

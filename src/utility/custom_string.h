#pragma once
// LANGUAGE: CwC
#include "object.h"
#include "serializer.h"

#include <cassert>
#include <cstring>
#include <string>

/** An immutable string class that wraps a character array.
 * The character array is zero terminated. The size() of the
 * String does count the terminator character. Most operations
 * work by copy, but there are exceptions (this is mostly to support
 * large strings and avoid them being copied).
 *  author: vitekj@me.com */
class String : public Object {
public:
  size_t size_; // number of characters excluding terminate (\0)
  char *cstr_;  // owned; char array

  /** Build a string from a string constant */
  String(char const *cstr, size_t len) {
    size_ = len;
    cstr_ = new char[size_ + 1];
    memcpy(cstr_, cstr, size_ + 1);
    cstr_[size_] = 0; // terminate
  }
  /** Builds a string from a char*, steal must be true, we do not copy!
   *  cstr must be allocated for len+1 and must be zero terminated. */
  String(bool steal, char *cstr, size_t len) {
    assert(steal && cstr[len] == 0);
    size_ = len;
    cstr_ = cstr;
  }

  explicit String(char const *cstr) : String(cstr, strlen(cstr)) {}

  /** Build a string from another String */
  String(String &from) : Object(from) {
    size_ = from.size_;
    cstr_ = new char[size_ + 1]; // ensure that we copy the terminator
    memcpy(cstr_, from.cstr_, size_ + 1);
  }

  /** Delete the string */
  ~String() override { delete[] cstr_; }

  /** Return the number characters in the string (does not count the terminator)
   */
  size_t size() { return size_; }

  /** Return the raw char*. The result should not be modified or freed. */
  char *c_str() override { return cstr_; }

  /** Returns the character at index */
  char at(size_t index) {
    assert(index < size_);
    return cstr_[index];
  }

  /** Compare two strings. */
  bool equals(Object *other) override {
    if (other == this)
      return true;
    String *x = dynamic_cast<String *>(other);
    if (x == nullptr)
      return false;
    if (size_ != x->size_)
      return false;
    return strncmp(cstr_, x->cstr_, size_) == 0;
  }

  /** Deep copy of this string */
  String *clone() override { return new String(*this); }

  /** This consumes cstr_, the String must be deleted next */
  char *steal() {
    char *res = cstr_;
    cstr_ = nullptr;
    return res;
  }

  /** Compute a hash for this string. */
  size_t hash_me() override {
    size_t hash = 0;
    for (size_t i = 0; i < size_; ++i)
      hash = cstr_[i] + (hash << 6) + (hash << 16) - hash;
    return hash;
  }

  size_t serialization_required_bytes() override {
    return Serializer::get_required_bytes(this->size_) + this->size_ + 1;
  }

  void serialize(Serializer &serializer) override {
    // Set the size of the string
    serializer.set_size_t(this->size_);

    // Now copy the string to the serializer, including the null terminator
    serializer.set_generic(this->cstr_, this->size_ + 1);
  }

  /**
   * Interprets a serialized piece of data as a string, and returns the value
   * as a string.
   * @param deserializer The deserializer containing the serialized data we
   *        are trying to deserialize_as_message.
   * @return String pulled from the serialized data buffer. Nullptr if it
   *            failed to retrieve a string from the deserializer
   */
  static String *deserialize_as_string(Deserializer &deserializer) {
    String *ret_value = nullptr;

    if (deserializer.has_size_t()) {
      // Get the expected string length
      size_t length = deserializer.get_size_t();

      // Verify that there is enough space left in the buffer to contain a
      // string of that length
      if (deserializer.get_num_bytes_left() >= length + 1) {
        // Build the string by grabbing the bytes in from the deserializer
        char *str = new char[length + 1];
        for (size_t i = 0; i < length + 1; i ++) {
          str[i] = static_cast<char>(deserializer.get_byte());
        }

        ret_value = new String(true, str, length);
      }
    }

    return ret_value;
  }
};

/** A string buffer builds a string from various pieces.
 *  author: jv */
class StrBuff : public Object {
public:
  char *val_; // owned; consumed by get()
  size_t capacity_;
  size_t size_;

  StrBuff() {
    val_ = new char[capacity_ = 10];
    size_ = 0;
  }
  void grow_by_(size_t step) {
    if (step + size_ < capacity_)
      return;
    capacity_ *= 2;
    if (step + size_ >= capacity_)
      capacity_ += step;
    char *oldV = val_;
    val_ = new char[capacity_];
    memcpy(val_, oldV, size_);
    delete[] oldV;
  }
  StrBuff &c(const char *str) {
    size_t step = strlen(str);
    grow_by_(step);
    memcpy(val_ + size_, str, step);
    size_ += step;
    return *this;
  }
  StrBuff &c(String &s) { return c(s.c_str()); }
  StrBuff &c(size_t v) { return c(std::to_string(v).c_str()); } // Cpp

  String *get() {
    assert(val_ != nullptr); // can be called only once
    grow_by_(1);             // ensure space for terminator
    val_[size_] = 0;         // terminate
    String *res = new String(true, val_, size_);
    val_ = nullptr; // val_ was consumed above
    return res;
  }
};

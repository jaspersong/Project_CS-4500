#pragma once
// LANGUAGE: CwC
#include "custom_object.h"
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
class String : public CustomObject {
public:
  /** Build a string from a string constant */
  String(char const *cstr, size_t len);

  /** Builds a string from a char*, steal must be true, we do not copy!
   *  cstr must be allocated for len+1 and must be zero terminated. */
  String(bool steal, char *cstr, size_t len);
  explicit String(char const *cstr);

  /** Build a string from another String */
  String(String &from);

  /** Delete the string */
  ~String() override;

  /** Return the number characters in the string (does not count the terminator)
   */
  size_t size() { return size_; }

  /** Return the raw char*. The result should not be modified or freed. */
  char *c_str() const override { return cstr_; }

  /** Returns the character at index */
  char at(size_t index);

  /** Compare two strings. */
  bool equals(CustomObject *other) const override;

  /** Deep copy of this string */
  String *clone() override { return new String(*this); }

  /** This consumes cstr_, the String must be deleted next */
  char *steal();

  /** Compute a hash for this string. */
  size_t hash_me() override;

  size_t serialization_required_bytes() override;
  void serialize(Serializer &serializer) override;

  /**
   * Interprets a serialized piece of data as a string, and returns the value
   * as a string.
   * @param deserializer The deserializer containing the serialized data we
   *        are trying to deserialize_as_message.
   * @return String pulled from the serialized data buffer. Nullptr if it
   *            failed to retrieve a string from the deserializer
   */
  static String *deserialize_as_string(Deserializer &deserializer);

private:
  size_t size_; // number of characters excluding terminate (\0)
  char *cstr_;  // owned; char array
};

/** A string buffer builds a string from various pieces.
 *  author: jv */
class StrBuff : public CustomObject {
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

#pragma once
#include "helper.h"
// LANGUAGE: CwC

class Serializer;

/** Base class for all objects in the system.
 *  author: vitekj@me.com */
class Object : public Sys {
public:
  size_t hash_; // every object has a hash, subclasses must implement the
                // functionality

  Object() { hash_ = 0; }

  /** Subclasses may have something to do on finalziation */
  virtual ~Object() {}

  /** Return the hash value of this object */
  size_t hash() { return hash_ != 0 ? hash_ : hash_ = hash_me(); }

  /** Subclasses should redefine */
  virtual bool equals(Object *other) { return this == other; }

  /** Return a copy of the object; nullptr is considered an error */
  virtual Object *clone() { return nullptr; }

  /** Compute the hash code (subclass responsibility) */
  virtual size_t hash_me() { return 1; };

  /** Returned c_str is owned by the object, don't modify nor delete. */
  virtual char *c_str() { return nullptr; }

  /**
   * Calculates the number of bytes is needed in order to serialize this object.
   * @return The number of bytes needed to serialize this object.
   */
  virtual size_t serialization_required_bytes() {
    return 0;
  }

  /**
   * Takes in a serializer and appends the objects serialized data to an
   * already-prepared buffer of serialized data.
   * @param serializer The serializer whose serialized data will be
   *          appended to
   */
  virtual void serialize(Serializer &serializer) {}
};

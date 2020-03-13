#pragma once

#include "custom_object.h"
#include "custom_string.h"
#include <stdlib.h>

// general purpose array that stores Objects
class Array : public CustomObject {
public:
  CustomObject ** container_; // Array of all the objects.
  size_t num_items_; // The number of continuous items in the array.
  size_t current_capacity_; // Current maximum amount of items array can hold

  static const size_t DEFAULT_CAPACITY = 16;

  // constructor
  Array() {
    this->container_ = new CustomObject *[DEFAULT_CAPACITY];
    this->num_items_ = 0;
    this->current_capacity_ = DEFAULT_CAPACITY;
  }

  // destructor
  ~Array() {
    delete[] this->container_;
  }

  // insert element at given index and push back rest of elements
  void insert(CustomObject *val, size_t index) {
    // size_t is a unsigned integer, which means that a negative value is
    // impossible. The value will always be 0 or greater. Therefore, there is
    // no need to check for invalid indices.

    if (index >= this->current_capacity_) {
      // We now need more space within the array in order to accomodate the
      // new value.
      size_t new_capacity = index + 2; // Add 2 to account for pushing back
      CustomObject ** new_container = new CustomObject *[new_capacity];

      // Now move all the items from the old container into the new container
      for (size_t i = 0; i < this->num_items_; i++) {
        new_container[i] = this->container_[i];
      }

      // Delete the old container and move onto the new
      delete[] this->container_;
      this->container_ = new_container;
      this->current_capacity_ = new_capacity;
    }

    if (index >= this->num_items_) {
      // The index provided is at the very end, or greater. No need for
      // pushback.
      this->num_items_ = index + 1;
      this->container_[index] = val;
    }
    else {
      // The index provided is in the middle of the rest of the array. Need
      // to push back.
      for (size_t i = this->num_items_; i > index; i--) {
        // Move the previous item into the current index
        this->container_[i] = this->container_[i - 1];
      }

      // Now we have the index spot free. Place the item into that index
      this->container_[index] = val;
      this->num_items_ += 1;
    }
  }

  // insert element at end of array
  void insertAtEnd(CustomObject *val) {
    this->insert(val, this->num_items_);
  }

  // insert multiple elements at index
  void insertMultiple(Array *vals, size_t index) {
    // Iterate through the provided array and slowly insert it at that index
    for (size_t i = 0; i < vals->getSize(); i++) {
      this->insert(vals->getElementAt(i), index + i);
    }
  }

  // set element at index to given value
  //return old value at this index
  CustomObject *set(CustomObject *val, size_t index) {
    CustomObject *ret_value = nullptr;
    if (index >= this->current_capacity_) {
      // This index is outside of the current capacity. We need to add more
      // space, so call insert. Because the index is past the capacity, there
      // will never be any items after the specified index, so we do not
      // worry about push back
      this->insert(val, index);
    }
    else {
      // Store the old value to be returned and then replace it
      ret_value = this->container_[index];
      this->container_[index] = val;

      // Update the number of items if the value is greater than the current
      // number of items
      if (index >= this->num_items_) {
        this->num_items_ = index + 1;
      }
    }

    return ret_value;
  }

  // clear Array
  void clear() {
    // Iterate through the array and set all the values to 0
    for (size_t i = 0; i < this->current_capacity_; i++) {
      this->container_[i] = nullptr;
    }

    // Update the number of items to 0 because the array is empty
    this->num_items_ = 0;
  }

  // remove object at given index and return that object
  CustomObject *remove(size_t index) {
    if (index >= this->num_items_) {
      fprintf(stderr, "Index is out of bounds.\n");
      exit(1);
    }
    else {
      CustomObject * ret_value = this->container_[index];

      // Iterate up the array in order to pull the other objects up the array.
      for (size_t i = index; i < this->num_items_ - 1; i++) {
        this->container_[i] = this->container_[i + 1];
      }

      // Decrement the number of items to account for the removal of this item.
      this->num_items_ -= 1;

      return ret_value;
    }
  }

  // return element at given index and error if out-of-bounds
  CustomObject *getElementAt(size_t index) {
    if (index >= this->num_items_) {
      fprintf(stderr, "Index is out of bounds.\n");
      exit(1);
    }
    else {
      return this->container_[index];
    }
  }

  // return size of Array
  size_t getSize() {
    return this->num_items_;
  }

  // first index of that element and -1 if doesn't exist
  int indexOf(CustomObject *val) {
    // Iterate through the array to find the element specified
    for (size_t i = 0; i < this->num_items_; i++) {
      if (val->equals(this->container_[i])) {
        return static_cast<int>(i);
      }
    }

    return -1; // Could not find the element.
  }

  // Override of the virtual function
  bool equals(CustomObject *const obj) {
    if (obj == nullptr) {
      return false;
    }

    Array *a = dynamic_cast<Array *>(obj);

    if (a == nullptr) {
      return false;
    }
    else if (this->num_items_ == a->num_items_) {
      // Iterate through the array and check to make sure they contain the
      // same objects
      for (size_t i = 0; i < this->num_items_; i++) {
        if (!this->container_[i]->equals(a->container_[i])) {
          // Found an item that does not match.
          return false;
        }
      }

      // Did not find any differing objects.
      return true;
    }
    else {
      return false;
    }
  }

  // Override of the virtual function
  size_t hash() {
    size_t ret_value = this->num_items_;

    // Iterate through the items to add the hash up
    for (size_t i = 0; i < this->num_items_; i++) {
      if (this->container_[i] != nullptr) {
        ret_value += this->container_[i]->hash();
      }
    }

    return ret_value;
  }
};

// type strict array that stores Strings
class StringArray : public CustomObject {
public:
  // Delegate array management to generalized obj array.
  Array * delegator_;

  // constructor
  StringArray() {
    this->delegator_ = new Array();
  }

  // destructor
  ~StringArray() override {
    delete this->delegator_;
  }

  // insert element at given index and push back rest of elements
  void insert(String *val, size_t index) {
    this->delegator_->insert(val, index);
  }

  // insert element at end of array
  void insertAtEnd(String *val) {
    this->delegator_->insertAtEnd(val);
  }

  // insert multiple elements at index
  void insertMultiple(StringArray *vals, size_t index) {
    this->delegator_->insertMultiple(vals->delegator_, index);
  }

  // set element at index to given value
  //return old value at this index
  String *set(String *val, size_t index) {
    CustomObject * ret_value = this->delegator_->set(val, index);

    return dynamic_cast<String *>(ret_value);
  }

  // clear Array
  void clear() {
    this->delegator_->clear();
  }

  // remove object at given index and return that object
  String *remove(size_t index) {
    CustomObject * ret_value = this->delegator_->remove(index);

    // Cast the return of the remove function because all of the contents in
    // this array is guaranteed to be a string.
    return dynamic_cast<String *>(ret_value);
  }

  // return element at given index and error if out-of-bounds
  String *getElementAt(size_t index) {
    CustomObject * ret_value = this->delegator_->getElementAt(index);

    // Cast the return of the getElementAt function because all of the
    // contents in this array is guaranteed to be a string
    return dynamic_cast<String *>(ret_value);
  }

  // return size of Array
  size_t getSize() {
    return this->delegator_->getSize();
  }

  // first index of that element and -1 if doesn't exist
  int indexOf(String *val) {
    return this->delegator_->indexOf(val);
  }

  // Override of the virtual function
  bool equals(CustomObject *const obj) override {
    if (obj == nullptr) {
      return false;
    }

    StringArray *a = dynamic_cast<StringArray *>(obj);
    return this->delegator_->equals(a->delegator_);
  }

  // Override of the virtual function
  size_t hash() {
    return this->delegator_->hash();
  }
};

// A boxed object representing a float value. This representation is used
// solely for the FloatArray.
class BoxedFloat_ : public CustomObject {
public:
  float val_;

  // Constructor
  BoxedFloat_(float value) {
    this->val_ = value;
  }

  // Deconstruct
  ~BoxedFloat_() {
    // Nothing to deconstruct
  }

  /* Returns whether two objects are equal, to be overriden by subclasses */
  bool equals(CustomObject * const obj) {
    if (obj == nullptr) {
      return false;
    }

    BoxedFloat_ *other = dynamic_cast<BoxedFloat_ *>(obj);

    if (other == nullptr) {
      return false;
    }

    return this->val_ == other->val_;
  }

  /* Returns an object's hash value. Identical objects should have identical
   * hashes */
  size_t hash() {
    return static_cast<size_t>(this->val_);
  }
};

// type strict array that stores floats
class FloatArray : public CustomObject {
public:
  // Delegate array management to generalized obj array.
  Array * delegator_;

  // constructor
  FloatArray() {
    this->delegator_ = new Array();
  }

  // destructor
  ~FloatArray() {
    // Iterate through the array and free all the boxed floats
    while (this->delegator_->getSize() > 0) {
      CustomObject * to_free = this->delegator_->remove(this->delegator_->getSize()
          - 1);
      delete to_free;
    }

    delete this->delegator_;
  }

  // insert element at given index and push back rest of elements
  void insert(float val, size_t index) {
    BoxedFloat_ * value = new BoxedFloat_(val);
    this->delegator_->insert(value, index);
  }

  // insert element at end of array
  void insertAtEnd(float val) {
    BoxedFloat_ * value = new BoxedFloat_(val);
    this->delegator_->insertAtEnd(value);
  }


  // insert multiple elements at index
  void insertMultiple(FloatArray *vals, size_t index) {
    this->delegator_->insertMultiple(vals->delegator_, index);
  }


  // set element at index to given value
  //return old value at this index
  float set(float val, size_t index) {
    BoxedFloat_ * value = new BoxedFloat_(val);
    CustomObject * ret_value = this->delegator_->set(value, index);

    // Get the old value from the set function
    BoxedFloat_ *ret_boxed_float = dynamic_cast<BoxedFloat_ *>(ret_value);
    float ret_float = 0.0f;

    // Check to make sure that the returned object is not a nullptr, in the
    // case where set did not have to replace an old value.
    if (ret_boxed_float != nullptr) {
      ret_float = ret_boxed_float->val_;
    }
    delete ret_boxed_float;
    return ret_float;
  }

  // clear Array
  void clear() {
    // Iterate through the delegator until it's empty, because we need to get
    // the objects in order to free them.
    while (this->delegator_->getSize() > 0) {
      CustomObject * to_free = this->delegator_->remove(this->delegator_->getSize()
                                                  - 1);
      delete to_free;
    }
  }

  // remove object at given index and return that object
  float remove(size_t index) {
    CustomObject * value = this->delegator_->remove(index);

    // Cast the value in order to get the float value.
    BoxedFloat_ * float_value = dynamic_cast<BoxedFloat_ *>(value);
    float ret_value = float_value->val_;

    // Delete the removed value.
    delete float_value;

    return ret_value;
  }

  // return element at given index and error if out-of-bounds
  float getElementAt(size_t index) {
    CustomObject * value = this->delegator_->getElementAt(index);

    // Cast the value in order to get the float value
    BoxedFloat_ * float_value = dynamic_cast<BoxedFloat_ *>(value);

    // No need to free the float value because it isn't removed from the array.

    return float_value->val_;
  }

  // return size of Array
  size_t getSize() {
    return this->delegator_->getSize();
  }

  // first index of that element and -1 if doesn't exist
  int indexOf(float val) {
    // Create a boxed float in order for it to be compared with all the other
    // boxed float objects within the array.
    BoxedFloat_ * value = new BoxedFloat_(val);
    int ret_value = this->delegator_->indexOf(value);

    // Now free the newly created boxed float because we don't need it anymore
    delete value;

    return ret_value;
  }

  // Override of the virtual function
  bool equals(CustomObject *const obj) {
    if (obj == nullptr) {
      return false;
    }

    FloatArray *a = dynamic_cast<FloatArray *>(obj);
    return this->delegator_->equals(a->delegator_);
  }

  // Override of the virtual function
  size_t hash() {
    return this->delegator_->hash();
  }
};

// A boxed object representing an integer value. This class is solely used
// for an IntArray.
class BoxedInt_ : public CustomObject {
public:
  int val_;

  // Constructor
  BoxedInt_(int value) {
    this->val_ = value;
  }

  // Deconstruct
  ~BoxedInt_() {
    // Nothing to deconstruct
  }

  /* Returns whether two objects are equal, to be overriden by subclasses */
  bool equals(CustomObject * const obj) {
    if (obj == nullptr) {
      return false;
    }

    BoxedInt_ *other = dynamic_cast<BoxedInt_ *>(obj);

    if (other == nullptr) {
      return false;
    }

    return this->val_ == other->val_;
  }

  /* Returns an object's hash value. Identical objects should have identical
   * hashes */
  size_t hash() {
    return static_cast<size_t>(this->val_);
  }
};

// type strict array that stores ints
class IntArray : public CustomObject {
public:
  // Delegate array management to generalized obj array.
  Array * delegator_;

  // constructor
  IntArray() {
    this->delegator_ = new Array();
  }

  // destructor
  ~IntArray() {
    // Iterate through the array and delete the boxed objects within the array
    while (this->delegator_->getSize() > 0) {
      CustomObject * to_free = this->delegator_->remove(this->delegator_->getSize()
                                                  - 1);
      delete to_free;
    }

    delete this->delegator_;
  }

  // insert element at given index and push back rest of elements
  void insert(int val, size_t index) {
    BoxedInt_ * value = new BoxedInt_(val);
    this->delegator_->insert(value, index);
  }

  // insert element at end of array
  void insertAtEnd(int val) {
    BoxedInt_ * value = new BoxedInt_(val);
    this->delegator_->insertAtEnd(value);
  }

  // insert multiple elements at index
  void insertMultiple(IntArray *vals, size_t index) {
    this->delegator_->insertMultiple(vals->delegator_, index);
  }

  // set element at index to given value
  //return old value at this index
  int set(int val, size_t index) {
    BoxedInt_ * value = new BoxedInt_(val);
    CustomObject * ret_value = this->delegator_->set(value, index);

    BoxedInt_ * ret_boxed_int = dynamic_cast<BoxedInt_ *>(ret_value);

    // Check to make sure that the returned object is not a nullptr, in the
    // case where set did not have to replace an old value.
    int ret_int = 0;
    if (ret_boxed_int != nullptr) {
      ret_int = ret_boxed_int->val_;
    }

    return ret_int;
  }

  // clear Array
  void clear() {
    while (this->delegator_->getSize() > 0) {
      CustomObject * to_free = this->delegator_->remove(this->delegator_->getSize()
                                                  - 1);
      delete to_free;
    }
  }

  // remove object at given index and return that object
  int remove(size_t index) {
    CustomObject * value = this->delegator_->remove(index);

    // Cast the value into a boxed integer in order to retrieve the integer.
    BoxedInt_ * int_value = dynamic_cast<BoxedInt_ *>(value);
    int ret_value = int_value->val_;
    delete int_value;

    return ret_value;
  }

  // return element at given index and error if out-of-bounds
  int getElementAt(size_t index) {
    CustomObject * value = this->delegator_->getElementAt(index);

    // Cast the value into a boxed integer in order to retrieve the integer.
    BoxedInt_ * int_value = dynamic_cast<BoxedInt_ *>(value);

    // No need to free the object because it is still inside of the array.

    return int_value->val_;
  }

  // return size of Array
  size_t getSize() {
    return this->delegator_->getSize();
  }

  // first index of that element and -1 if doesn't exist
  int indexOf(int val) {
    BoxedInt_ * value = new BoxedInt_(val);
    int ret_value = this->delegator_->indexOf(value);

    // Free the boxed integer because we don't need it anymore
    delete value;

    return ret_value;
  }

  // Override of the virtual function
  bool equals(CustomObject *const obj) {
    if (obj == nullptr) {
      return false;
    }

    IntArray *a = dynamic_cast<IntArray *>(obj);
    return this->delegator_->equals(a->delegator_);
  }

  // Override of the virtual function
  size_t hash() {
    return this->delegator_->hash();
  }
};

// A boxed object representing a boolean value. This class is solely used
// for an BoolArray.
class BoxedBool_ : public CustomObject {
public:
  bool val_;

  // Constructor
  BoxedBool_(bool value) {
    this->val_ = value;
  }

  // Deconstruct
  ~BoxedBool_() {
    // Nothing to deconstruct
  }

  /* Returns whether two objects are equal, to be overriden by subclasses */
  bool equals(CustomObject * const obj) {
    if (obj == nullptr) {
      return false;
    }

    BoxedBool_ *other = dynamic_cast<BoxedBool_ *>(obj);

    if (other == nullptr) {
      return false;
    }

    return this->val_ == other->val_;
  }

  /* Returns an object's hash value. Identical objects should have identical
   * hashes */
  size_t hash() {
    return static_cast<size_t>(this->val_);
  }
};

// type strict array that stores booleans
class BoolArray : public CustomObject {
public:
  // Delegate array management to generalized obj array.
  Array * delegator_;

  // constructor
  BoolArray() {
    this->delegator_ = new Array();
  }

  // destructor
  ~BoolArray() {
    // Iterate through the array and delete the boxed objects within the array
    while (this->delegator_->getSize() > 0) {
      CustomObject * to_free = this->delegator_->remove(this->delegator_->getSize()
                                                  - 1);
      delete to_free;
    }

    delete this->delegator_;
  }

  // insert element at given index and push back rest of elements
  void insert(bool val, size_t index) {
    BoxedBool_ * value = new BoxedBool_(val);
    this->delegator_->insert(value, index);
  }

  // insert element at end of array
  void insertAtEnd(bool val) {
    BoxedBool_ * value = new BoxedBool_(val);
    this->delegator_->insertAtEnd(value);
  }

  // insert multiple elements at index
  void insertMultiple(BoolArray *vals, size_t index) {
    this->delegator_->insertMultiple(vals->delegator_, index);
  }

  // set element at index to given value
  //return old value at this index
  bool set(bool val, size_t index) {
    BoxedBool_ * value = new BoxedBool_(val);
    CustomObject * ret_value = this->delegator_->set(value, index);

    BoxedBool_ * ret_boxed_bool = dynamic_cast<BoxedBool_ *>(ret_value);

    // Check to make sure that the returned object is not a nullptr, in the
    // case where set did not have to replace an old value.
    bool ret_bool = false;
    if (ret_boxed_bool != nullptr) {
      ret_bool = ret_boxed_bool->val_;
    }

    return ret_bool;
  }

  // clear Array
  void clear() {
    // Iterate through the array and delete the boxed objects within the array
    while (this->delegator_->getSize() > 0) {
      CustomObject * to_free = this->delegator_->remove(this->delegator_->getSize()
                                                  - 1);
      delete to_free;
    }
  }

  // remove object at given index and return that object
  bool remove(size_t index) {
    CustomObject * value = this->delegator_->remove(index);

    // Cast the removed item into a boxed boolean in order to retrieve the value
    BoxedBool_ * bool_value = dynamic_cast<BoxedBool_ *>(value);
    bool ret_value = bool_value->val_;
    delete bool_value;

    return ret_value;
  }

  // return element at given index and error if out-of-bounds
  bool getElementAt(size_t index) {
    CustomObject * value = this->delegator_->getElementAt(index);

    // Cast the removed item into a boxed boolean in order to retrieve the value
    BoxedBool_ * bool_value = dynamic_cast<BoxedBool_ *>(value);

    // No need to free the boolean value because it is still in the array.

    return bool_value->val_;
  }

  // return size of Array
  size_t getSize() {
    return this->delegator_->getSize();
  }

  // first index of that element and -1 if doesn't exist
  int indexOf(bool val) {
    BoxedBool_ * value = new BoxedBool_(val);
    int ret_value = this->delegator_->indexOf(value);

    // Now remove the boxed bool because we don't need it anymore
    delete value;

    return ret_value;
  }

  // Override of the virtual function
  bool equals(CustomObject *const obj) {
    if (obj == nullptr) {
      return false;
    }

    BoolArray *a = dynamic_cast<BoolArray *>(obj);
    return this->delegator_->equals(a->delegator_);
  }

  // Override of the virtual function
  size_t hash() {
    return this->delegator_->hash();
  }
};
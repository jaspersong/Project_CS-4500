/**
 * Name: Snowy Chen, Joe Song
 * Date: 21 March 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::Cpp

#include "key.h"

Key::Key(const char *name, size_t home_id) {
  this->name = new String(name);
  this->home_id = home_id;
}

Key::~Key() { delete this->name; }

size_t Key::hash_me() { return this->home_id + this->name->hash(); }

bool Key::equals(CustomObject *other) const {
  if (other == this)
    return true;
  Key *x = dynamic_cast<Key *>(other);
  if (x == nullptr)
    return false;
  return (this->home_id == x->home_id) && this->name->equals(x->name);
}

void Key::serialize(Serializer &serializer) {
  serializer.set_size_t(this->home_id);
  this->name->serialize(serializer);
}

size_t Key::serialization_required_bytes() {
  return Serializer::get_required_bytes(this->home_id) +
         this->name->serialization_required_bytes();
}

Key *Key::deserialize_as_key(Deserializer &deserializer) {
  Key *ret_value = nullptr;

  if (deserializer.has_size_t()) {
    // Get the home id
    size_t home_id = deserializer.get_size_t();

    // Get the key name
    String *str = String::deserialize_as_string(deserializer);

    // Create the key and delete any interim memory required
    ret_value = new Key(str->c_str(), home_id);
    delete str;
  }

  return ret_value;
}

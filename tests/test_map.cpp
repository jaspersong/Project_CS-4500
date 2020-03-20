#include "custom_object.h"
#include "custom_string.h"
#include "map.h"

bool it_returns_the_value_for_a_key() {
  Map *map = new Map();
  String *foo_key = new String("fooKey");
  String *foo = new String("foo");
  map->put(foo_key, foo);

  bool ret_value = map->get(foo_key)->equals(foo);
  delete map;
  delete foo_key;
  delete foo;

  return ret_value;
}

bool it_contains_the_key() {
  Map *map = new Map();
  String *foo_key = new String("fooKey");
  String *foo = new String("foo");
  map->put(foo_key, foo);

  bool ret_value = map->contains_key(foo_key) && !map->contains_key(foo);
  delete map;
  delete foo_key;
  delete foo;
  return ret_value;
}

bool it_equals_another_empty_map() {
  Map *map1 = new Map();
  Map *map2 = new Map();

  bool ret_value = map1->equals(map2);
  delete map1;
  delete map2;
  return ret_value;
}

bool it_equals_another_map() {
  Map *map1 = new Map();
  Map *map2 = new Map();
  String *foo_key = new String("fooKey");
  String *foo = new String("foo");
  map1->put(foo_key, foo);
  map2->put(foo_key, foo);

  bool ret_value = map1->equals(map2);
  delete map1;
  delete map2;
  delete foo_key;
  delete foo;
  return ret_value;
}

bool it_computes_the_same_empty_hashcode() {
  Map *map1 = new Map();
  Map *map2 = new Map();

  bool ret_value = map1->hash() == map2->hash();
  delete map1;
  delete map2;
  return ret_value;
}

bool it_computes_the_same_hashcode() {
  Map *map1 = new Map();
  Map *map2 = new Map();
  String *foo_key = new String("fooKey");
  String *foo = new String("foo");
  map1->put(foo_key, foo);
  map2->put(foo_key, foo);

  bool ret_value = map1->hash() == map2->hash();
  delete map1;
  delete map2;
  delete foo_key;
  delete foo;
  return ret_value;
}

bool it_computes_keyset() {
  Map *map = new Map();
  String *foo_key = new String("fooKey");
  String *bar_key = new String("barKey");
  String *foo = new String("foo");
  String *bar = new String("bar");
  map->put(foo_key, foo);
  map->put(bar_key, bar);
  CustomObject **keys = map->key_set();

  bool containsFoo = false;
  bool containsBar = false;
  for (int i = 0; i < map->size(); i++) {
    if (keys[i]->equals(foo_key)) {
      containsFoo = true;
    }
    if (keys[i]->equals(bar_key)) {
      containsBar = true;
    }
  }

  bool ret_value = containsFoo && containsBar && map->size() == 2;
  delete map;
  delete foo_key;
  delete bar_key;
  delete foo;
  delete bar;
  delete[] keys;
  return ret_value;
}

bool it_removes_elements() {
  Map *map = new Map();
  String *foo_key = new String("fooKey");
  String *bar_key = new String("barKey");
  String *foo = new String("foo");
  String *bar = new String("bar");
  map->put(foo_key, foo);
  map->put(bar_key, bar);

  if (map->size() != 2)
    return false;

  CustomObject *foo_removed = map->remove(foo_key);
  if (map->size() != 1)
    return false;
  if (!foo->equals(foo_removed))
    return false;

  CustomObject *bar_removed = map->remove(bar_key);
  if (map->size() != 0)
    return false;

  bool ret_value = bar->equals(bar_removed);
  delete map;
  delete foo_key;
  delete bar_key;
  delete foo;
  delete bar;
  return ret_value;
}

bool it_computes_size() {
  Map *map = new Map();
  if (map->size() != 0)
    return false;
  String *foo_key = new String("fooKey");
  String *foo = new String("foo");
  map->put(foo_key, foo);

  bool ret_value = map->size() == 1;
  delete map;
  delete foo_key;
  delete foo;
  return ret_value;
}

bool it_updates_values() {
  Map *map = new Map();
  String *foo_key = new String("fooKey");
  String *foo = new String("foo");
  String *bar = new String("bar");

  map->put(foo_key, foo);
  if (!map->get(foo_key)->equals(foo))
    return false;

  map->put(foo_key, bar);
  bool ret_value = map->get(foo_key)->equals(bar);
  delete map;
  delete foo_key;
  delete foo;
  delete bar;
  return ret_value;
}

int main() {
  bool success = it_returns_the_value_for_a_key() && it_contains_the_key() &&
                 it_equals_another_empty_map() && it_equals_another_map() &&
                 it_computes_the_same_empty_hashcode() &&
                 it_computes_the_same_hashcode() && it_computes_keyset() &&
                 it_removes_elements() && it_computes_size() &&
                 it_updates_values();
  return success ? 0 : 1;
}
#include "custom_object.h"
#include "custom_string.h"
#include "helper.h"
#include "map.h"

Sys helper;

void it_returns_the_value_for_a_key() {
  Map *map = new Map();
  String *foo_key = new String("fooKey");
  String *foo = new String("foo");
  map->put(foo_key, foo);

  helper.t_true(map->get(foo_key)->equals(foo));

  delete map;
  delete foo_key;
  delete foo;

  helper.OK("Test 0 passed");
}

void it_contains_the_key() {
  Map *map = new Map();
  String *foo_key = new String("fooKey");
  String *foo = new String("foo");
  map->put(foo_key, foo);

  helper.t_true(map->contains_key(foo_key) && !map->contains_key(foo));

  delete map;
  delete foo_key;
  delete foo;

  helper.OK("Test 1 passed");
}

void it_equals_another_empty_map() {
  Map *map1 = new Map();
  Map *map2 = new Map();

  helper.t_true(map1->equals(map2));

  delete map1;
  delete map2;

  helper.OK("Test 2 passed");
}

void it_equals_another_map() {
  Map *map1 = new Map();
  Map *map2 = new Map();
  String *foo_key = new String("fooKey");
  String *foo = new String("foo");
  map1->put(foo_key, foo);
  map2->put(foo_key, foo);

  helper.t_true(map1->equals(map2));

  delete map1;
  delete map2;
  delete foo_key;
  delete foo;

  helper.OK("Test 3 passed");
}

void it_computes_the_same_empty_hashcode() {
  Map *map1 = new Map();
  Map *map2 = new Map();

  helper.t_true(map1->hash() == map2->hash());

  delete map1;
  delete map2;

  helper.OK("Test 4 passed");
}

void it_computes_the_same_hashcode() {
  Map *map1 = new Map();
  Map *map2 = new Map();
  String *foo_key = new String("fooKey");
  String *foo = new String("foo");
  map1->put(foo_key, foo);
  map2->put(foo_key, foo);

  helper.t_true(map1->hash() == map2->hash());

  delete map1;
  delete map2;
  delete foo_key;
  delete foo;

  helper.OK("Test 5 passed");
}

void it_computes_keyset() {
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

  helper.t_true(containsFoo);
  helper.t_true(containsBar);
  helper.t_true(map->size() == 2);

  delete map;
  delete foo_key;
  delete bar_key;
  delete foo;
  delete bar;
  delete[] keys;

  helper.OK("Test 6 passed");
}

void it_removes_elements() {
  Map *map = new Map();
  String *foo_key = new String("fooKey");
  String *bar_key = new String("barKey");
  String *foo = new String("foo");
  String *bar = new String("bar");
  map->put(foo_key, foo);
  map->put(bar_key, bar);

  helper.t_false(map->size() != 2);

  CustomObject *foo_removed = map->remove(foo_key);
  helper.t_false(map->size() != 1);
  helper.t_false(!foo->equals(foo_removed));

  CustomObject *bar_removed = map->remove(bar_key);
  helper.t_false(map->size() != 0);

  helper.t_true(bar->equals(bar_removed));

  delete map;
  delete foo_key;
  delete bar_key;
  delete foo;
  delete bar;

  helper.OK("Test 7 passed");
}

void it_computes_size() {
  Map *map = new Map();
  helper.t_false(map->size() != 0);

  String *foo_key = new String("fooKey");
  String *foo = new String("foo");
  map->put(foo_key, foo);

  helper.t_true(map->size() == 1);

  delete map;
  delete foo_key;
  delete foo;

  helper.OK("Test 8 passed");
}

void it_updates_values() {
  Map *map = new Map();
  String *foo_key = new String("fooKey");
  String *foo = new String("foo");
  String *bar = new String("bar");

  map->put(foo_key, foo);
  helper.t_false(!map->get(foo_key)->equals(foo));

  map->put(foo_key, bar);
  helper.t_true(map->get(foo_key)->equals(bar));

  delete map;
  delete foo_key;
  delete foo;
  delete bar;

  helper.OK("Test 9 passed");
}

int main() {
  it_returns_the_value_for_a_key();
  it_contains_the_key();
  it_equals_another_empty_map();
  it_equals_another_map();
  it_computes_the_same_empty_hashcode();
  it_computes_the_same_hashcode();
  it_computes_keyset();
  it_removes_elements();
  it_computes_size();
  it_updates_values();
  return 0;
}
/**
 * Name: Snowy Chen, Joe Song
 * Date: 21 February 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// lang::Cpp

#include "custom_string.h"
#include "dataframe.h"
#include "helper.h"
#include "key.h"
#include "key_value_store.h"

Sys helper;

void test_from_array() {
  String str0("Then the fire nation attacked.");
  String str1("Hello");
  String str2("World");
  String str3(":(");
  String str4(":)");

  Key key0("0", 0);
  Key key1("1", 0);
  Key key2("2", 0);
  Key key3("3", 0);
  Key key4("4", 0);

  KeyValueStore map(1);

  size_t SZ = 10;
  bool bool_vals[SZ];
  int int_vals[SZ];
  float float_vals[SZ];
  String *str_vals[SZ];

  // Populate the arrays
  for (size_t i = 0; i < SZ; i++) {
    bool_vals[i] = (i % 2 == 0);
    int_vals[i] = static_cast<int>(i);
    float_vals[i] = static_cast<float>(i + 1);
    str_vals[i] = nullptr;
  }
  str_vals[0] = &str0;
  str_vals[1] = &str1;
  str_vals[2] = &str2;
  str_vals[3] = &str3;
  str_vals[4] = &str4;
  str_vals[9] = &str0;

  // Make the dataframes and put them into the map
  DataFrame *df0 = KeyValueStore::from_array(key0, &map, SZ, bool_vals);
  DataFrame *df1 = KeyValueStore::from_array(key1, &map, SZ, int_vals);
  DataFrame *df2 = KeyValueStore::from_array(key2, &map, SZ, float_vals);
  DataFrame *df3 = KeyValueStore::from_array(key3, &map, SZ, str_vals);

  helper.t_true(df0->get_schema().width() == 1);
  helper.t_true(df1->get_schema().width() == 1);
  helper.t_true(df2->get_schema().width() == 1);
  helper.t_true(df3->get_schema().width() == 1);
  helper.t_true(df0->get_schema().col_type(0) == ColumnType_Bool);
  helper.t_true(df1->get_schema().col_type(0) == ColumnType_Integer);
  helper.t_true(df2->get_schema().col_type(0) == ColumnType_Float);
  helper.t_true(df3->get_schema().col_type(0) == ColumnType_String);
  for (size_t i = 0; i < SZ; i++) {
    helper.t_true(df0->get_bool(0, i) == (i % 2 == 0));
    helper.t_true(df1->get_int(0, i) == static_cast<int>(i));
    helper.t_true(df2->get_float(0, i) == static_cast<float>(i + 1));
  }
  helper.t_true(df3->get_string(0, 0)->equals(&str0));
  helper.t_true(df3->get_string(0, 1)->equals(&str1));
  helper.t_true(df3->get_string(0, 2)->equals(&str2));
  helper.t_true(df3->get_string(0, 3)->equals(&str3));
  helper.t_true(df3->get_string(0, 4)->equals(&str4));
  helper.t_true(df3->get_string(0, 5) == nullptr);
  helper.t_true(df3->get_string(0, 6) == nullptr);
  helper.t_true(df3->get_string(0, 7) == nullptr);
  helper.t_true(df3->get_string(0, 8) == nullptr);
  helper.t_true(df3->get_string(0, 9)->equals(&str0));

  // TODO: Create and test functions for if the KV-store contains the key
  //  locally, or if it's in a different KV-store

  auto *exp_df0 = reinterpret_cast<DataFrame *>(map.get_local(key0));
  auto *exp_df1 = reinterpret_cast<DataFrame *>(map.get_local(key1));
  auto *exp_df2 = reinterpret_cast<DataFrame *>(map.get_local(key2));
  auto *exp_df3 = reinterpret_cast<DataFrame *>(map.get_local(key3));

  // Should be true with pointer equality, since they should be pointing to
  // the same exact objects.
  helper.t_true(exp_df0->equals(df0));
  helper.t_true(exp_df1->equals(df1));
  helper.t_true(exp_df2->equals(df2));
  helper.t_true(exp_df3->equals(df3));

  delete df0;
  delete df1;
  delete df2;
  delete df3;

  helper.OK("Test 1 passed");
}

void test_from_scalar() {
  String str0("Then the fire nation attacked.");

  Key key0("0", 0);
  Key key1("1", 0);
  Key key2("2", 0);
  Key key3("3", 0);
  Key key4("4", 0);

  KeyValueStore map(1);

  // Make the dataframes and put them into the map
  DataFrame *df0 = KeyValueStore::from_scalar(key0, &map, true);
  DataFrame *df1 = KeyValueStore::from_scalar(key1, &map, 5);
  DataFrame *df2 = KeyValueStore::from_scalar(key2, &map, 6.3f);
  DataFrame *df3 = KeyValueStore::from_scalar(key3, &map, &str0);

  helper.t_true(df0->get_schema().width() == 1);
  helper.t_true(df1->get_schema().width() == 1);
  helper.t_true(df2->get_schema().width() == 1);
  helper.t_true(df3->get_schema().width() == 1);
  helper.t_true(df0->get_schema().col_type(0) == ColumnType_Bool);
  helper.t_true(df1->get_schema().col_type(0) == ColumnType_Integer);
  helper.t_true(df2->get_schema().col_type(0) == ColumnType_Float);
  helper.t_true(df3->get_schema().col_type(0) == ColumnType_String);
  helper.t_true(df0->nrows() == 1);
  helper.t_true(df1->nrows() == 1);
  helper.t_true(df2->nrows() == 1);
  helper.t_true(df3->nrows() == 1);

  helper.t_true(df0->get_bool(0, 0));
  helper.t_true(df1->get_int(0, 0) == 5);
  helper.t_true(df2->get_float(0, 0) == 6.3f);
  helper.t_true(df3->get_string(0, 0)->equals(&str0));

  // TODO: Create and test functions for if the KV-store contains the key
  //  locally, or if it's in a different KV-store

  auto *exp_df0 = reinterpret_cast<DataFrame *>(map.get_local(key0));
  auto *exp_df1 = reinterpret_cast<DataFrame *>(map.get_local(key1));
  auto *exp_df2 = reinterpret_cast<DataFrame *>(map.get_local(key2));
  auto *exp_df3 = reinterpret_cast<DataFrame *>(map.get_local(key3));

  // Should be true with pointer equality, since they should be pointing to
  // the same exact objects.
  helper.t_true(exp_df0->equals(df0));
  helper.t_true(exp_df1->equals(df1));
  helper.t_true(exp_df2->equals(df2));
  helper.t_true(exp_df3->equals(df3));

  delete df0;
  delete df1;
  delete df2;
  delete df3;

  helper.OK("Test 2 passed");
}

void test_wait_get() {
  String str0("Then the fire nation attacked.");
  Key key0("0", 0);
  KeyValueStore map(1);

  DataFrame *df = KeyValueStore::from_scalar(key0, &map, &str0);
  DataFrame *copy_df = map.wait_and_get(key0);

  helper.t_false(df == copy_df);
  helper.t_true(df->get_schema().col_type(0) == copy_df->get_schema().col_type(0));
  helper.t_true(df->nrows() == copy_df->nrows());
  helper.t_true(df->ncols() == copy_df->ncols());
  helper.t_true(df->get_string(0, 0)->equals(copy_df->get_string(0, 0)));

  delete df;
  delete copy_df;

  helper.OK("Test 3 passed");
}

int main(int argc, char **argv) {
  test_from_array();
  test_from_scalar();
  test_wait_get();
}
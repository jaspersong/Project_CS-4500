/**
 * Name: Snowy Chen, Joe Song
 * Date: 21 February 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// lang::Cpp

#include "helper.h"
#include "custom_string.h"
#include "dataframe.h"

Sys helper;

void basic() {
  Schema s("II");

  DataFrame df(s);
  Row  r(df.get_schema());
  for(size_t i = 0; i <  1000 * 1000; i++) {
    r.set(0,(int)i);
    r.set(1,(int)i+1);
    df.add_row(r);
  }

  helper.t_true(df.get_int((size_t)0,1) == 1);

  helper.OK("Test 0 passed");
}

void test1() {
  String str_hello("Hello");
  String str_world("World");
  String str_software("Software");
  String str_dev("Development");
  String str_avatar("Then the fire nation attacked.");

  // Create a schema for the rows
  Schema * s1 = new Schema();
  s1->add_column('B');
  s1->add_column('I');
  s1->add_column('F');
  s1->add_column('S');
  s1->add_column('S');
  s1->add_column('B');

  // Construct a dummy column
  DF_Column * bool_column = new DF_BoolColumn(2, false, true);

  // Construct the row
  Row * r0 = new Row(*s1);
  Row * r1 = new Row(*s1);
  Row * r2 = new Row(*s1);
  Row * r3 = new Row(*s1);
  r0->set(0, false);
  r0->set(1, 3);
  r0->set(2, 4.2f);
  r0->set(4, &str_avatar);
  r0->set(5, false);
  r1->set(0, true);
  r1->set(1, 4);
  r1->set(2, 5.3f);
  r1->set(3, &str_hello);
  r1->set(4, &str_world);
  r1->set(5, false);
  r3->set(3, &str_software);
  r3->set(4, &str_dev);

  // Construct the dataframe
  DataFrame * df0 = new DataFrame(*s1);

  // Add the rows
  df0->add_row(*r0);
  df0->add_row(*r1);
  df0->add_row(*r2);
  df0->add_row(*r3);

  // Test the get/set values functions
  helper.t_false(df0->get_bool(0, 0));
  helper.t_true(df0->get_int(1, 0) == 3);
  helper.t_true(df0->get_float(2, 0) == 4.2f);
  helper.t_true(df0->get_string(4, 0)->equals(&str_avatar));
  helper.t_false(df0->get_bool(5, 0));
  helper.t_true(df0->get_bool(0, 1));
  helper.t_true(df0->get_int(1, 1) == 4);
  helper.t_true(df0->get_float(2, 1) == 5.3f);
  helper.t_true(df0->get_string(3, 1)->equals(&str_hello));
  helper.t_true(df0->get_string(4, 1)->equals(&str_world));
  helper.t_false(df0->get_bool(5, 1));
  helper.t_true(df0->get_string(3, 3)->equals(&str_software));
  helper.t_true(df0->get_string(4, 3)->equals(&str_dev));
  df0->set(0, 0, true);
  df0->set(1, 0, 4);
  df0->set(2, 1, 3.3f);
  df0->set(4, 3, &str_avatar);
  helper.t_true(df0->get_bool(0, 0));
  helper.t_true(df0->get_int(1, 0) == 4);
  helper.t_true(df0->get_float(2, 1) == 3.3f);
  helper.t_true(df0->get_string(4, 3)->equals(&str_avatar));
  df0->set(0, 2, false);
  df0->set(1, 2, 20);
  df0->set(2, 2, 80.9f);
  df0->set(4, 2, &str_world);
  helper.t_false(df0->get_bool(0, 2));
  helper.t_true(df0->get_int(1, 2) == 20);
  helper.t_true(df0->get_float(2, 2) == 80.9f);
  helper.t_true(df0->get_string(4, 2)->equals(&str_world));

  // Test adding row information in bulk
  helper.t_true(df0->nrows() == 4);
  df0->add_row(*r3);
  helper.t_true(df0->nrows() == 5);
  helper.t_true(df0->get_string(3, 4)->equals(&str_software));
  helper.t_true(df0->get_string(4, 4)->equals(&str_dev));
  df0->fill_row(0, *r3);
  helper.t_true(df0->get_string(3, 0)->equals(&str_software));
  helper.t_true(df0->get_string(4, 0)->equals(&str_dev));

  // Test adding new columns
  helper.t_true(df0->ncols() == 6);
  df0->add_column(bool_column);
  df0->add_column(bool_column);
  helper.t_true(df0->ncols() == 8);
  helper.t_false(df0->get_bool(6, 0));
  helper.t_true(df0->get_bool(6, 1));
  helper.t_false(df0->get_bool(7, 0));
  helper.t_true(df0->get_bool(7, 1));

  // Deconstruct the memory accordingly
  delete s1;
  delete bool_column;
  delete r0;
  delete r1;
  delete r2;
  delete r3;
  delete df0;

  helper.OK("Test 1 passed");
}

// Tests the functionality of the API functions for Schema
void test2() {
  String str_hello("Hello");
  String str_world("World");
  String str_software("Software");
  String str_dev("Development");
  String str_avatar("Then the fire nation attacked.");

  Schema * s1 = new Schema();

  // Add some columns
  helper.t_true(s1->width() == 0);
  s1->add_column('B');
  s1->add_column('I');
  s1->add_column('F');
  s1->add_column('S');
  s1->add_column('S');
  s1->add_column('B');

  // Test the getter functions for columns
  helper.t_true(s1->col_type(0) == 'B');
  helper.t_true(s1->col_type(1) == 'I');
  helper.t_true(s1->col_type(2) == 'F');
  helper.t_true(s1->col_type(3) == 'S');
  helper.t_true(s1->col_type(4) == 'S');
  helper.t_true(s1->col_type(5) == 'B');
  helper.t_true(s1->width() == 6);

  // Test the copy constructor
  Schema * copy = new Schema(*s1);
  helper.t_true(copy->col_type(0) == 'B');
  helper.t_true(copy->col_type(1) == 'I');
  helper.t_true(copy->col_type(2) == 'F');
  helper.t_true(copy->col_type(3) == 'S');
  helper.t_true(copy->col_type(4) == 'S');
  helper.t_true(copy->col_type(5) == 'B');
  helper.t_true(copy->width() == 6);
  helper.t_true(s1->width() == 6);
  copy->add_column('F');
  helper.t_true(copy->width() == 7);
  helper.t_true(s1->width() == 6);

  // Test the string of types constructor
  Schema * s2 = new Schema("IFSS");
  helper.t_true(s2->col_type(0) == 'I');
  helper.t_true(s2->col_type(1) == 'F');
  helper.t_true(s2->col_type(2) == 'S');
  helper.t_true(s2->col_type(3) == 'S');
  helper.t_true(s2->width() == 4);

  // Free the memory accordingly
  delete s1;
  delete s2;
  delete copy;

  helper.OK("Test 2 passed");
}

// Test the functionality of the Column API functions
void test3() {
  // Create some test strings
  String str_hello("Hello");
  String str_world("World");
  String str_software("Software");
  String str_dev("Development");
  String str_avatar("Then the fire nation attacked.");

  // Create all the column types
  DF_Column * bool_column = new DF_BoolColumn();
  DF_Column * int_column = new DF_IntColumn();
  DF_Column * float_column = new DF_FloatColumn();
  DF_Column *string_column = new DF_StringColumn();

  // Test the get column type
  helper.t_true(bool_column->get_type() == 'B');
  helper.t_true(int_column->get_type() == 'I');
  helper.t_true(float_column->get_type() == 'F');
  helper.t_true(string_column->get_type() == 'S');

  // Add values to the column
  bool_column->push_back(true);
  bool_column->push_back(false);
  int_column->push_back(0);
  int_column->push_back(10);
  int_column->push_back(20);
  float_column->push_back(10.0f);
  float_column->push_back(20.5f);
  float_column->push_back(46.2f);
  float_column->push_back(0.0f);
  string_column->push_back(&str_hello);
  string_column->push_back(&str_world);
  string_column->push_back(&str_software);
  string_column->push_back(&str_dev);
  string_column->push_back(&str_avatar);

  // Verify the sizes of each of the column
  helper.t_true(bool_column->size() == 2);
  helper.t_true(int_column->size() == 3);
  helper.t_true(float_column->size() == 4);
  helper.t_true(string_column->size() == 5);

  // Verify that we get the same column under its correct type
  helper.t_true(bool_column->equals(bool_column->as_bool()));
  helper.t_true(bool_column->as_int() == nullptr);
  helper.t_true(bool_column->as_float() == nullptr);
  helper.t_true(bool_column->as_string() == nullptr);
  helper.t_true(int_column->as_bool() == nullptr);
  helper.t_true(int_column->equals(int_column->as_int()));
  helper.t_true(int_column->as_float() == nullptr);
  helper.t_true(int_column->as_string() == nullptr);
  helper.t_true(string_column->as_bool() == nullptr);
  helper.t_true(string_column->as_int() == nullptr);
  helper.t_true(string_column->as_float() == nullptr);
  helper.t_true(string_column->equals(string_column->as_string()));

  // Now get the column under its actual type to start testing primitive
  // functions
  DF_BoolColumn * conv_bool_column = bool_column->as_bool();
  DF_IntColumn * conv_int_column = int_column->as_int();
  DF_FloatColumn * conv_float_column = float_column->as_float();
  DF_StringColumn * conv_string_colunmn = string_column->as_string();

  // Test the boolean column
  helper.t_true(conv_bool_column->get(0));
  helper.t_false(conv_bool_column->get(1));
  conv_bool_column->set(1, true);
  helper.t_true(conv_bool_column->get(1));
  conv_bool_column->push_back(false);
  helper.t_false(conv_bool_column->get(2));
  helper.t_true(conv_bool_column->size() == 3);

  // Test the int column
  helper.t_true(conv_int_column->get(0) == 0);
  helper.t_true(conv_int_column->get(1) == 10);
  helper.t_true(conv_int_column->get(2) == 20);
  conv_int_column->set(0, 40);
  helper.t_true(conv_int_column->get(0) == 40);
  conv_int_column->push_back(74);
  helper.t_true(conv_int_column->get(3) == 74);
  helper.t_true(conv_int_column->size() == 4);

  // Test the float column
  helper.t_true(conv_float_column->get(0) == 10.0f);
  helper.t_true(conv_float_column->get(1) == 20.5f);
  helper.t_true(conv_float_column->get(2) == 46.2f);
  helper.t_true(conv_float_column->get(3) == 0.0f);
  conv_float_column->set(3, 0.01f);
  helper.t_true(conv_float_column->get(3) == 0.01f);
  conv_float_column->push_back(4.2f);
  helper.t_true(conv_float_column->get(4) == 4.2f);
  helper.t_true(conv_float_column->size() == 5);

  // Test the string column
  helper.t_true(conv_string_colunmn->get(0)->equals(&str_hello));
  helper.t_true(conv_string_colunmn->get(1)->equals(&str_world));
  helper.t_true(conv_string_colunmn->get(2)->equals(&str_software));
  helper.t_true(conv_string_colunmn->get(3)->equals(&str_dev));
  helper.t_true(conv_string_colunmn->get(4)->equals(&str_avatar));
  conv_string_colunmn->set(0, &str_avatar);
  helper.t_true(conv_string_colunmn->get(0)->equals(&str_avatar));
  conv_string_colunmn->push_back(&str_hello);
  helper.t_true(conv_string_colunmn->get(5)->equals(&str_hello));
  helper.t_true(conv_string_colunmn->size() == 6);

  // Test the constructors
  DF_BoolColumn * cons_bool_column = new DF_BoolColumn(2, false, true);
  DF_IntColumn * cons_int_column = new DF_IntColumn(3, 0, 1, 2);
  DF_FloatColumn * cons_float_column = new DF_FloatColumn(4, 0.1f, 0.2f, 0.3f, 0.4f);
  DF_StringColumn * cons_string_column = new DF_StringColumn(3, &str_hello,
      &str_world, &str_avatar);
  helper.t_true(cons_bool_column->get(1));
  helper.t_false((cons_bool_column->get(0)));
  cons_bool_column->push_back(false);
  helper.t_true(cons_bool_column->size() == 3);
  helper.t_false(cons_bool_column->get(2));
  helper.t_true(cons_int_column->get(0) == 0);
  helper.t_true(cons_int_column->get(1) == 1);
  helper.t_true(cons_int_column->get(2) == 2);
  cons_int_column->push_back(100);
  helper.t_true(cons_int_column->get(3) == 100);
  helper.t_true(cons_int_column->size() == 4);
  helper.t_true(cons_float_column->get(0) == 0.1f);
  helper.t_true(cons_float_column->get(1) == 0.2f);
  helper.t_true(cons_float_column->get(2) == 0.3f);
  helper.t_true(cons_float_column->get(3) == 0.4f);
  cons_float_column->push_back(0.5f);
  helper.t_true(cons_float_column->get(4) == 0.5f);
  helper.t_true(cons_float_column->size() == 5);
  helper.t_true(cons_string_column->get(0)->equals(&str_hello));
  helper.t_true(cons_string_column->get(1)->equals(&str_world));
  helper.t_true((cons_string_column->get(2)->equals(&str_avatar)));
  cons_string_column->push_back(&str_software);
  helper.t_true(cons_string_column->get(3)->equals(&str_software));
  helper.t_true(cons_string_column->size() == 4);

  // Free memory accordingly
  delete bool_column;
  delete int_column;
  delete float_column;
  delete string_column;
  delete cons_bool_column;
  delete cons_int_column;
  delete cons_float_column;
  delete cons_string_column;

  helper.OK("Test 3 passed");
}

// Tests the API functions of the Rows
void test4() {
  String str_hello("Hello");
  String str_world("World");
  String str_software("Software");
  String str_dev("Development");
  String str_avatar("Then the fire nation attacked.");

  // Create a schema for the rows
  Schema * s1 = new Schema();
  s1->add_column('B');
  s1->add_column('I');
  s1->add_column('F');
  s1->add_column('S');
  s1->add_column('S');
  s1->add_column('B');

  // Construct the row
  Row * r1 = new Row(*s1);

  // Test the setter/getter functions
  r1->set(0, true);
  r1->set(1, 4);
  r1->set(2, 5.3f);
  r1->set(3, &str_hello);
  r1->set(4, &str_world);
  r1->set(5, false);
  helper.t_true(r1->get_bool(0));
  helper.t_true(r1->get_int(1) == 4);
  helper.t_true(r1->get_float(2) == 5.3f);
  helper.t_true(r1->get_string(3)->equals(&str_hello));
  helper.t_true(r1->get_string(4)->equals(&str_world));
  helper.t_false(r1->get_bool(5));

  // Row schema tests
  helper.t_true(r1->col_type(0) == 'B');
  helper.t_true(r1->col_type(1) == 'I');
  helper.t_true(r1->col_type(2) == 'F');
  helper.t_true(r1->col_type(3) == 'S');
  helper.t_true(r1->col_type(4) == 'S');
  helper.t_true(r1->col_type(5) == 'B');
  helper.t_true(r1->width() == 6);

  // Test the set/get index of the row
  r1->set_idx(1);
  helper.t_true(r1->get_idx() == 1);

  // Free the memory accordingly
  delete s1;
  delete r1;

  helper.OK("Test 4 passed");
}

void test6() {
  // Create a dataframe
  Schema s("II");
  DataFrame df(s);

  // Add a few items into the dataframe
  Row r(df.get_schema());
  for(size_t i = 0; i < 100; i++) {
    r.set(0, static_cast<int>(i));
    r.set(1, static_cast<int>(i + 1));

    // Add the row to the dataframe
    df.add_row(r);
  }

  // Create a column with a few numbers as well
  DF_IntColumn c;
  for (size_t i = 0; i < 100; i++) {
    c.push_back(static_cast<int>(i + 3));

    // Check to make sure it was added correctly
    helper.t_true(c.get(i) == static_cast<int>(i + 3));
  }

  // Now add the column 4 times to the dataframe
  df.add_column(&c);
  df.add_column(&c);
  df.add_column(&c);
  df.add_column(&c);

  helper.t_true(df.ncols() == 6);
  helper.t_true(df.nrows() == 100);

  // Ensure that the values are correct
  for (size_t i = 0; i < 100; i++) {
    helper.t_true(df.get_int(0, i) == static_cast<int>(i));
    helper.t_true(df.get_int(1, i) == static_cast<int>(i + 1));
    helper.t_true(df.get_int(2, i) == static_cast<int>(i + 3));
    helper.t_true(df.get_int(3, i) == static_cast<int>(i + 3));
    helper.t_true(df.get_int(4, i) == static_cast<int>(i + 3));
    helper.t_true(df.get_int(5, i) == static_cast<int>(i + 3));

    helper.t_true(c.get(i) == static_cast<int>(i + 3));
  }

  // Change some of the values of the last column of the dataframe
  df.set(5, 0, 42);
  df.set(5, 1, 42);
  df.set(5, 2, 42);
  df.set(5, 3, 42);

  // Verify that the values were changed appropriately
  for (size_t i = 0; i < 4; i++) {
    helper.t_true(df.get_int(0, i) == static_cast<int>(i));
    helper.t_true(df.get_int(1, i) == static_cast<int>(i + 1));
    helper.t_true(df.get_int(2, i) == static_cast<int>(i + 3));
    helper.t_true(df.get_int(3, i) == static_cast<int>(i + 3));
    helper.t_true(df.get_int(4, i) == static_cast<int>(i + 3));
    helper.t_true(df.get_int(5, i) == static_cast<int>(42));
  }

  helper.OK("Test 6 passed");
}

void test_from_array() {
  String str0("Then the fire nation attacked.");
  String str1("Hello");
  String str2("World");
  String str3(":(");
  String str4(":)");

  Key key0("0", 0);
  Key key1("1", 1);
  Key key2("2", 2);
  Key key3("3", 3);
  Key key4("4", 4);

  Map map;

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
  DataFrame *df0 = DataFrame::fromArray(&key0, &map, SZ, bool_vals);
  DataFrame *df1 = DataFrame::fromArray(&key1, &map, SZ, int_vals);
  DataFrame *df2 = DataFrame::fromArray(&key2, &map, SZ, float_vals);
  DataFrame *df3 = DataFrame::fromArray(&key3, &map, SZ, str_vals);

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

  helper.t_true(map.contains_key(&key0));
  helper.t_true(map.contains_key(&key1));
  helper.t_true(map.contains_key(&key2));
  helper.t_true(map.contains_key(&key3));
  helper.t_false(map.contains_key(&key4));

  DataFrame *exp_df0 = reinterpret_cast<DataFrame *>(map.get(&key0));
  DataFrame *exp_df1 = reinterpret_cast<DataFrame *>(map.get(&key1));
  DataFrame *exp_df2 = reinterpret_cast<DataFrame *>(map.get(&key2));
  DataFrame *exp_df3 = reinterpret_cast<DataFrame *>(map.get(&key3));

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

  helper.OK("Test 5 passed");
}

int main(int argc, char **argv) {
  basic();
  test1();
  test2();
  test3();
  test4();
  test_from_array();
  test6();
}

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

Sys helper;

void basic() {
  Schema s("II");

  DataFrame df(s);
  Row r(df.get_schema());
  for (size_t i = 0; i < 1000 * 1000; i++) {
    r.set(0, (int)i);
    r.set(1, (int)i + 1);
    df.add_row(r);
  }

  helper.t_true(df.get_int((size_t)0, 1) == 1);

  helper.OK("Test 0 passed");
}

void test1() {
  String str_hello("Hello");
  String str_world("World");
  String str_software("Software");
  String str_dev("Development");
  String str_avatar("Then the fire nation attacked.");

  // Create a schema for the rows
  auto *s1 = new Schema();
  s1->add_column('B');
  s1->add_column('I');
  s1->add_column('F');
  s1->add_column('S');
  s1->add_column('S');
  s1->add_column('B');

  // Construct a dummy column
  auto *bool_column = new DF_Column(ColumnType_Bool);
  bool_column->push_back(false);
  bool_column->push_back(true);

  // Construct the row
  Row *r0 = new Row(*s1);
  Row *r1 = new Row(*s1);
  Row *r2 = new Row(*s1);
  Row *r3 = new Row(*s1);
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
  auto *df0 = new DataFrame(*s1);

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

  auto *s1 = new Schema();

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
  auto *copy = new Schema(*s1);
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
  auto *s2 = new Schema("IFSS");
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
  auto *bool_column = new DF_Column(ColumnType_Bool);
  auto *int_column = new DF_Column(ColumnType_Integer);
  auto *float_column = new DF_Column(ColumnType_Float);
  auto *string_column = new DF_Column(ColumnType_String);

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

  // Test the boolean column
  helper.t_true(bool_column->get_bool(0));
  helper.t_false(bool_column->get_bool(1));
  bool_column->set(1, true);
  helper.t_true(bool_column->get_bool(1));
  bool_column->push_back(false);
  helper.t_false(bool_column->get_bool(2));
  helper.t_true(bool_column->size() == 3);

  // Test the int column
  helper.t_true(int_column->get_int(0) == 0);
  helper.t_true(int_column->get_int(1) == 10);
  helper.t_true(int_column->get_int(2) == 20);
  int_column->set(0, 40);
  helper.t_true(int_column->get_int(0) == 40);
  int_column->push_back(74);
  helper.t_true(int_column->get_int(3) == 74);
  helper.t_true(int_column->size() == 4);

  // Test the float column
  helper.t_true(float_column->get_float(0) == 10.0f);
  helper.t_true(float_column->get_float(1) == 20.5f);
  helper.t_true(float_column->get_float(2) == 46.2f);
  helper.t_true(float_column->get_float(3) == 0.0f);
  float_column->set(3, 0.01f);
  helper.t_true(float_column->get_float(3) == 0.01f);
  float_column->push_back(4.2f);
  helper.t_true(float_column->get_float(4) == 4.2f);
  helper.t_true(float_column->size() == 5);

  // Test the string column
  helper.t_true(string_column->get_string(0)->equals(&str_hello));
  helper.t_true(string_column->get_string(1)->equals(&str_world));
  helper.t_true(string_column->get_string(2)->equals(&str_software));
  helper.t_true(string_column->get_string(3)->equals(&str_dev));
  helper.t_true(string_column->get_string(4)->equals(&str_avatar));
  string_column->set(0, &str_avatar);
  helper.t_true(string_column->get_string(0)->equals(&str_avatar));
  string_column->push_back(&str_hello);
  helper.t_true(string_column->get_string(5)->equals(&str_hello));
  helper.t_true(string_column->size() == 6);

  // Test the constructors
  auto *cons_bool_column = new DF_Column(ColumnType_Bool);
  cons_bool_column->push_back(false);
  cons_bool_column->push_back(true);
  auto *cons_int_column = new DF_Column(ColumnType_Integer);
  cons_int_column->push_back(0);
  cons_int_column->push_back(1);
  cons_int_column->push_back(2);
  auto *cons_float_column = new DF_Column(ColumnType_Float);
  cons_float_column->push_back(0.1f);
  cons_float_column->push_back(0.2f);
  cons_float_column->push_back(0.3f);
  cons_float_column->push_back(0.4f);
  auto *cons_string_column = new DF_Column(ColumnType_String);
  cons_string_column->push_back(&str_hello);
  cons_string_column->push_back(&str_world);
  cons_string_column->push_back(&str_avatar);
  helper.t_true(cons_bool_column->get_bool(1));
  helper.t_false((cons_bool_column->get_bool(0)));
  cons_bool_column->push_back(false);
  helper.t_true(cons_bool_column->size() == 3);
  helper.t_false(cons_bool_column->get_bool(2));
  helper.t_true(cons_int_column->get_int(0) == 0);
  helper.t_true(cons_int_column->get_int(1) == 1);
  helper.t_true(cons_int_column->get_int(2) == 2);
  cons_int_column->push_back(100);
  helper.t_true(cons_int_column->get_int(3) == 100);
  helper.t_true(cons_int_column->size() == 4);
  helper.t_true(cons_float_column->get_float(0) == 0.1f);
  helper.t_true(cons_float_column->get_float(1) == 0.2f);
  helper.t_true(cons_float_column->get_float(2) == 0.3f);
  helper.t_true(cons_float_column->get_float(3) == 0.4f);
  cons_float_column->push_back(0.5f);
  helper.t_true(cons_float_column->get_float(4) == 0.5f);
  helper.t_true(cons_float_column->size() == 5);
  helper.t_true(cons_string_column->get_string(0)->equals(&str_hello));
  helper.t_true(cons_string_column->get_string(1)->equals(&str_world));
  helper.t_true((cons_string_column->get_string(2)->equals(&str_avatar)));
  cons_string_column->push_back(&str_software);
  helper.t_true(cons_string_column->get_string(3)->equals(&str_software));
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
  auto *s1 = new Schema();
  s1->add_column('B');
  s1->add_column('I');
  s1->add_column('F');
  s1->add_column('S');
  s1->add_column('S');
  s1->add_column('B');

  // Construct the row
  Row *r1 = new Row(*s1);

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

  // Free the memory accordingly
  delete s1;
  delete r1;

  helper.OK("Test 4 passed");
}

void test5() {
  // Create a dataframe
  Schema s("II");
  DataFrame df(s);

  // Add a few items into the dataframe
  Row r(df.get_schema());
  for (size_t i = 0; i < 100; i++) {
    r.set(0, static_cast<int>(i));
    r.set(1, static_cast<int>(i + 1));

    // Add the row to the dataframe
    df.add_row(r);
  }

  // Create a column with a few numbers as well
  DF_Column c(ColumnType_Integer);
  for (size_t i = 0; i < 100; i++) {
    c.push_back(static_cast<int>(i + 3));

    // Check to make sure it was added correctly
    helper.t_true(c.get_int(i) == static_cast<int>(i + 3));
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

    helper.t_true(c.get_int(i) == static_cast<int>(i + 3));
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

  helper.OK("Test 5 passed");
}

void test6() {
  // Create a dataframe
  String str("Hello");
  Schema s("BIFS");
  DataFrame df(s);

  // Add a few items into the dataframe
  Row r(df.get_schema());
  for (size_t i = 0; i < 5; i++) {
    r.set(0, true);
    r.set(1, 5);
    r.set(2, 6.3f);
    r.set(3, &str);

    // Add the row to the dataframe
    df.add_row(r);
  }

  // Now test round trip serialization/deserialization
  Serializer serializer;
  df.serialize(serializer);

  unsigned char *buffer = serializer.get_serialized_buffer();
  helper.t_true(df.serialization_required_bytes() == serializer.get_size_serialized_data());

  Deserializer deserializer(buffer, serializer.get_size_serialized_data());
  DataFrame *ret_df = DataFrame::deserialize_as_dataframe(deserializer);

  // Now make sure that it's the same the original
  helper.t_true(df.nrows() == ret_df->nrows());
  helper.t_true(df.ncols() == ret_df->ncols());
  helper.t_true(df.get_schema().col_type(0) == ret_df->get_schema().col_type(0));
  helper.t_true(df.get_schema().col_type(1) == ret_df->get_schema().col_type(1));
  helper.t_true(df.get_schema().col_type(2) == ret_df->get_schema().col_type(2));
  helper.t_true(df.get_schema().col_type(3) == ret_df->get_schema().col_type(3));
  for (size_t i = 0; i < 5; i++) {
    helper.t_true(df.get_bool(0, i) == ret_df->get_bool(0, i));
    helper.t_true(df.get_int(1, i) == ret_df->get_int(1, i));
    helper.t_true(df.get_float(2, i) == ret_df->get_float(2, i));
    helper.t_true(df.get_string(3, i)->equals(ret_df->get_string(3, i)));
  }

  delete[] buffer;
  delete ret_df;

  helper.OK("Test 6 passed");
}

int main(int argc, char **argv) {
  basic();
  test1();
  test2();
  test3();
  test4();
  test5();
  test6();
}

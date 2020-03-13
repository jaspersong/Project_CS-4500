/**
 * Name: Snowy Chen, Joe Song
 * Date: 21 February 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// lang::Cpp

#include <gtest/gtest.h>

#include "helper.h"
#include "custom_string.h"
#include "dataframe.h"
#include "summation_rower.h"
#include "euclidean_magnitude_rower.h"

#define ASSERT_T(a) ASSERT_EQ((a), true)
#define ASSERT_F(a) ASSERT_EQ((a), false)
#define ASSERT_EXIT_ZERO(a) ASSERT_EXIT(a(), ::testing::ExitedWithCode(0), ".*")

#define ASSERT_EXIT_ONE(a) ASSERT_EXIT(a(), ::testing::ExitedWithCode(1), ".*")

void test_str_accept_case() {
  // create different string types
  String str_homework("homework");
  String str_hard("hard");

  // create string types of schema
  Schema schema_string("S");

  // create a dataframe
  DataFrame df(schema_string);
  Row r(df.get_schema());

  // add row to the dataframe
  df.add_row(r);

  // test the accept function and print out an error message
  SummationFielder fielder(r);
  fielder.accept(&str_homework);

  exit(0);
}

void test_bool_accept_case() {
  // create boolean types of schema
  Schema schema_bool("B");

  // create a dataframe
  DataFrame df(schema_bool);
  Row r(df.get_schema());

  // add row to the dataframe
  df.add_row(r);

  // test the accept function and print out an error message
  SummationFielder fielder(r);
  fielder.accept(false);
  exit(0);
}

void test_float_accept_case() {
  // create float types of schema
  Schema schema_float("F");

  // create a dataframe
  DataFrame df(schema_float);
  Row r(df.get_schema());
  double expected_summation = 0.0;

  // add row to the dataframe
  df.add_row(r);
  r.set(0, 2.5f);
  expected_summation = 2.5;

  // test the accept function by passing in a float value
  SummationFielder fielder(r);
  fielder.start(r.get_idx());
  fielder.done();
  ASSERT_EQ(fielder.get_summation(), expected_summation);

  exit(0);
}

void test_int_accept_case() {
  // create int types of schema
  Schema schema_int("I");

  // create a dataframe
  DataFrame df(schema_int);
  Row r(df.get_schema());
  double expected_summation = 0.0;

  // add row to the dataframe
  df.add_row(r);
  r.set(0, 1);
  expected_summation = 1;
  // test the accept function by passing in a float value
  SummationFielder fielder(r);
  fielder.start(r.get_idx());
  fielder.done();
  ASSERT_EQ(fielder.get_summation(), expected_summation);
  exit(0);
}

void basic() {
  Schema s("II");

  DataFrame df(s);
  Row  r(df.get_schema());
  for(size_t i = 0; i <  1000 * 1000; i++) {
    r.set(0,(int)i);
    r.set(1,(int)i+1);
    df.add_row(r);
  }
  ASSERT_EQ(df.get_int((size_t)0,1), 1);
  exit(0);
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
  ASSERT_F(df0->get_bool(0, 0));
  ASSERT_EQ(df0->get_int(1, 0), 3);
  ASSERT_EQ(df0->get_float(2, 0), 4.2f);
  ASSERT_T(df0->get_string(4, 0)->equals(&str_avatar));
  ASSERT_F(df0->get_bool(5, 0));
  ASSERT_T(df0->get_bool(0, 1));
  ASSERT_EQ(df0->get_int(1, 1), 4);
  ASSERT_EQ(df0->get_float(2, 1), 5.3f);
  ASSERT_T(df0->get_string(3, 1)->equals(&str_hello));
  ASSERT_T(df0->get_string(4, 1)->equals(&str_world));
  ASSERT_F(df0->get_bool(5, 1));
  ASSERT_T(df0->get_string(3, 3)->equals(&str_software));
  ASSERT_T(df0->get_string(4, 3)->equals(&str_dev));
  df0->set(0, 0, true);
  df0->set(1, 0, 4);
  df0->set(2, 1, 3.3f);
  df0->set(4, 3, &str_avatar);
  ASSERT_T(df0->get_bool(0, 0));
  ASSERT_EQ(df0->get_int(1, 0), 4);
  ASSERT_EQ(df0->get_float(2, 1), 3.3f);
  ASSERT_T(df0->get_string(4, 3)->equals(&str_avatar));
  df0->set(0, 2, false);
  df0->set(1, 2, 20);
  df0->set(2, 2, 80.9f);
  df0->set(4, 2, &str_world);
  ASSERT_F(df0->get_bool(0, 2));
  ASSERT_EQ(df0->get_int(1, 2), 20);
  ASSERT_EQ(df0->get_float(2, 2), 80.9f);
  ASSERT_T(df0->get_string(4, 2)->equals(&str_world));

  // Test adding row information in bulk
  ASSERT_EQ(df0->nrows(), 4);
  df0->add_row(*r3);
  ASSERT_EQ(df0->nrows(), 5);
  ASSERT_T(df0->get_string(3, 4)->equals(&str_software));
  ASSERT_T(df0->get_string(4, 4)->equals(&str_dev));
  df0->fill_row(0, *r3);
  ASSERT_T(df0->get_string(3, 0)->equals(&str_software));
  ASSERT_T(df0->get_string(4, 0)->equals(&str_dev));

  // Test adding new columns
  ASSERT_EQ(df0->ncols(), 6);
  df0->add_column(bool_column);
  df0->add_column(bool_column);
  ASSERT_EQ(df0->ncols(), 8);
  ASSERT_F(df0->get_bool(6, 0));
  ASSERT_T(df0->get_bool(6, 1));
  ASSERT_F(df0->get_bool(7, 0));
  ASSERT_T(df0->get_bool(7, 1));

  // Deconstruct the memory accordingly
  delete s1;
  delete r0;
  delete r1;
  delete r2;
  delete r3;
  delete df0;

  exit(0);
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
  ASSERT_EQ(s1->width(), 0);
  s1->add_column('B');
  s1->add_column('I');
  s1->add_column('F');
  s1->add_column('S');
  s1->add_column('S');
  s1->add_column('B');

  // Test the getter functions for columns
  ASSERT_EQ(s1->col_type(0), 'B');
  ASSERT_EQ(s1->col_type(1), 'I');
  ASSERT_EQ(s1->col_type(2), 'F');
  ASSERT_EQ(s1->col_type(3), 'S');
  ASSERT_EQ(s1->col_type(4), 'S');
  ASSERT_EQ(s1->col_type(5), 'B');
  ASSERT_EQ(s1->width(), 6);

  // Test the copy constructor
  Schema * copy = new Schema(*s1);
  ASSERT_EQ(copy->col_type(0), 'B');
  ASSERT_EQ(copy->col_type(1), 'I');
  ASSERT_EQ(copy->col_type(2), 'F');
  ASSERT_EQ(copy->col_type(3), 'S');
  ASSERT_EQ(copy->col_type(4), 'S');
  ASSERT_EQ(copy->col_type(5), 'B');
  ASSERT_EQ(copy->width(), 6);
  ASSERT_EQ(s1->width(), 6);
  copy->add_column('F');
  ASSERT_EQ(copy->width(), 7);
  ASSERT_EQ(s1->width(), 6);

  // Test the string of types constructor
  Schema * s2 = new Schema("IFSS");
  ASSERT_EQ(s2->col_type(0), 'I');
  ASSERT_EQ(s2->col_type(1), 'F');
  ASSERT_EQ(s2->col_type(2), 'S');
  ASSERT_EQ(s2->col_type(3), 'S');
  ASSERT_EQ(s2->width(), 4);

  // Free the memory accordingly
  delete s1;

  exit(0);
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
  ASSERT_EQ(bool_column->get_type(), 'B');
  ASSERT_EQ(int_column->get_type(), 'I');
  ASSERT_EQ(float_column->get_type(), 'F');
  ASSERT_EQ(string_column->get_type(), 'S');

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
  ASSERT_EQ(bool_column->size(), 2);
  ASSERT_EQ(int_column->size(), 3);
  ASSERT_EQ(float_column->size(), 4);
  ASSERT_EQ(string_column->size(), 5);

  // Verify that we get the same column under its correct type
  ASSERT_T(bool_column->equals(bool_column->as_bool()));
  ASSERT_EQ(bool_column->as_int(), nullptr);
  ASSERT_EQ(bool_column->as_float(), nullptr);
  ASSERT_EQ(bool_column->as_string(), nullptr);
  ASSERT_EQ(int_column->as_bool(), nullptr);
  ASSERT_T(int_column->equals(int_column->as_int()));
  ASSERT_EQ(int_column->as_float(), nullptr);
  ASSERT_EQ(int_column->as_string(), nullptr);
  ASSERT_EQ(string_column->as_bool(), nullptr);
  ASSERT_EQ(string_column->as_int(), nullptr);
  ASSERT_EQ(string_column->as_float(), nullptr);
  ASSERT_T(string_column->equals(string_column->as_string()));

  // Now get the column under its actual type to start testing primitive
  // functions
  DF_BoolColumn * conv_bool_column = bool_column->as_bool();
  DF_IntColumn * conv_int_column = int_column->as_int();
  DF_FloatColumn * conv_float_column = float_column->as_float();
  DF_StringColumn * conv_string_colunmn = string_column->as_string();

  // Test the boolean column
  ASSERT_T(conv_bool_column->get(0));
  ASSERT_F(conv_bool_column->get(1));
  conv_bool_column->set(1, true);
  ASSERT_T(conv_bool_column->get(1));
  conv_bool_column->push_back(false);
  ASSERT_F(conv_bool_column->get(2));
  ASSERT_EQ(conv_bool_column->size(), 3);

  // Test the int column
  ASSERT_EQ(conv_int_column->get(0), 0);
  ASSERT_EQ(conv_int_column->get(1), 10);
  ASSERT_EQ(conv_int_column->get(2), 20);
  conv_int_column->set(0, 40);
  ASSERT_EQ(conv_int_column->get(0), 40);
  conv_int_column->push_back(74);
  ASSERT_EQ(conv_int_column->get(3), 74);
  ASSERT_EQ(conv_int_column->size(), 4);

  // Test the float column
  ASSERT_EQ(conv_float_column->get(0), 10.0f);
  ASSERT_EQ(conv_float_column->get(1), 20.5f);
  ASSERT_EQ(conv_float_column->get(2), 46.2f);
  ASSERT_EQ(conv_float_column->get(3), 0.0f);
  conv_float_column->set(3, 0.01f);
  ASSERT_EQ(conv_float_column->get(3), 0.01f);
  conv_float_column->push_back(4.2f);
  ASSERT_EQ(conv_float_column->get(4), 4.2f);
  ASSERT_EQ(conv_float_column->size(), 5);

  // Test the string column
  ASSERT_T(conv_string_colunmn->get(0)->equals(&str_hello));
  ASSERT_T(conv_string_colunmn->get(1)->equals(&str_world));
  ASSERT_T(conv_string_colunmn->get(2)->equals(&str_software));
  ASSERT_T(conv_string_colunmn->get(3)->equals(&str_dev));
  ASSERT_T(conv_string_colunmn->get(4)->equals(&str_avatar));
  conv_string_colunmn->set(0, &str_avatar);
  ASSERT_T(conv_string_colunmn->get(0)->equals(&str_avatar));
  conv_string_colunmn->push_back(&str_hello);
  ASSERT_T(conv_string_colunmn->get(5)->equals(&str_hello));
  ASSERT_EQ(conv_string_colunmn->size(), 6);

  // Test the constructors
  DF_BoolColumn * cons_bool_column = new DF_BoolColumn(2, false, true);
  DF_IntColumn * cons_int_column = new DF_IntColumn(3, 0, 1, 2);
  DF_FloatColumn * cons_float_column = new DF_FloatColumn(4, 0.1f, 0.2f, 0.3f, 0.4f);
  DF_StringColumn * cons_string_column = new DF_StringColumn(3, &str_hello,
      &str_world, &str_avatar);
  ASSERT_T(cons_bool_column->get(1));
  ASSERT_F((cons_bool_column->get(0)));
  cons_bool_column->push_back(false);
  ASSERT_EQ(cons_bool_column->size(), 3);
  ASSERT_F(cons_bool_column->get(2));
  ASSERT_EQ(cons_int_column->get(0), 0);
  ASSERT_EQ(cons_int_column->get(1), 1);
  ASSERT_EQ(cons_int_column->get(2), 2);
  cons_int_column->push_back(100);
  ASSERT_EQ(cons_int_column->get(3), 100);
  ASSERT_EQ(cons_int_column->size(), 4);
  ASSERT_EQ(cons_float_column->get(0), 0.1f);
  ASSERT_EQ(cons_float_column->get(1), 0.2f);
  ASSERT_EQ(cons_float_column->get(2), 0.3f);
  ASSERT_EQ(cons_float_column->get(3), 0.4f);
  cons_float_column->push_back(0.5f);
  ASSERT_EQ(cons_float_column->get(4), 0.5f);
  ASSERT_EQ(cons_float_column->size(), 5);
  ASSERT_T(cons_string_column->get(0)->equals(&str_hello));
  ASSERT_T(cons_string_column->get(1)->equals(&str_world));
  ASSERT_T((cons_string_column->get(2)->equals(&str_avatar)));
  cons_string_column->push_back(&str_software);
  ASSERT_T(cons_string_column->get(3)->equals(&str_software));
  ASSERT_EQ(cons_string_column->size(), 4);

  // Free memory accordingly
  delete bool_column;
  delete int_column;
  delete float_column;
  delete string_column;
  delete cons_bool_column;
  delete cons_int_column;
  delete cons_float_column;
  delete cons_string_column;

  exit(0);
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
  ASSERT_T(r1->get_bool(0));
  ASSERT_EQ(r1->get_int(1), 4);
  ASSERT_EQ(r1->get_float(2), 5.3f);
  ASSERT_T(r1->get_string(3)->equals(&str_hello));
  ASSERT_T(r1->get_string(4)->equals(&str_world));
  ASSERT_F(r1->get_bool(5));

  // Row schema tests
  ASSERT_EQ(r1->col_type(0), 'B');
  ASSERT_EQ(r1->col_type(1), 'I');
  ASSERT_EQ(r1->col_type(2), 'F');
  ASSERT_EQ(r1->col_type(3), 'S');
  ASSERT_EQ(r1->col_type(4), 'S');
  ASSERT_EQ(r1->col_type(5), 'B');
  ASSERT_EQ(r1->width(), 6);

  // Test the set/get index of the row
  r1->set_idx(1);
  ASSERT_EQ(r1->get_idx(), 1);

  // Free the memory accordingly
  delete s1;
  delete r1;

  exit(0);
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
    ASSERT_EQ(c.get(i), static_cast<int>(i + 3));
  }

  // Now add the column 4 times to the dataframe
  df.add_column(&c);
  df.add_column(&c);
  df.add_column(&c);
  df.add_column(&c);

  ASSERT_EQ(df.ncols(), 6);
  ASSERT_EQ(df.nrows(), 100);

  // Ensure that the values are correct
  for (size_t i = 0; i < 100; i++) {
    ASSERT_EQ(df.get_int(0, i), static_cast<int>(i));
    ASSERT_EQ(df.get_int(1, i), static_cast<int>(i + 1));
    ASSERT_EQ(df.get_int(2, i), static_cast<int>(i + 3));
    ASSERT_EQ(df.get_int(3, i), static_cast<int>(i + 3));
    ASSERT_EQ(df.get_int(4, i), static_cast<int>(i + 3));
    ASSERT_EQ(df.get_int(5, i), static_cast<int>(i + 3));

    ASSERT_EQ(c.get(i), static_cast<int>(i + 3));
  }

  // Change some of the values of the last column of the dataframe
  df.set(5, 0, 42);
  df.set(5, 1, 42);
  df.set(5, 2, 42);
  df.set(5, 3, 42);

  // Verify that the values were changed appropriately
  for (size_t i = 0; i < 4; i++) {
    ASSERT_EQ(df.get_int(0, i), static_cast<int>(i));
    ASSERT_EQ(df.get_int(1, i), static_cast<int>(i + 1));
    ASSERT_EQ(df.get_int(2, i), static_cast<int>(i + 3));
    ASSERT_EQ(df.get_int(3, i), static_cast<int>(i + 3));
    ASSERT_EQ(df.get_int(4, i), static_cast<int>(i + 3));
    ASSERT_EQ(df.get_int(5, i), static_cast<int>(42));
  }

  exit(0);
}

void test7() {
  Schema s("IFIF");

  // Create a very large dataframe
  DataFrame df(s);
  Row  r(df.get_schema());
  double expected_summation = 0.0;
  for(size_t i = 0; i <  1000 * 1000; i++) {
    r.set(0, (int)i);
    r.set(1, 0.5f);
    r.set(2, 1);
    r.set(3, 2.5f);

    expected_summation += static_cast<double>(i) + 0.5 + 1.0 + 2.5;

    // Add the row to the dataframe
    df.add_row(r);
  }

  // Filter the data frame for all of the rows that sum up a value greater
  // than 1000 * 100
  SummationGreaterThanRower rower(1000 * 100);
  DataFrame * result = df.filter(rower);

  // Verify that the rower summation is correct
  ASSERT_EQ(rower.get_summation(), expected_summation);

  // Verify that the resulting dataframe is correct
  ASSERT_EQ(result->get_int(0, 0), 1000 * 100 - 3);
  ASSERT_EQ(result->nrows(), 1000 * 1000 - (1000 * 100 - 3));

  // Clear the memory as necessary
  delete result;

  exit(0);
}

void test_pmap() {
  Schema s("IFIF");

  // Create a very large dataframe
  DataFrame df(s);
  Row  r(df.get_schema());
  double expected_summation = 0.0;
  for(size_t i = 0; i <  1000 * 1000; i++) {
    r.set(0, (int)i);
    r.set(1, 0.5f);
    r.set(2, 1);
    r.set(3, 2.5f);

    expected_summation += static_cast<double>(i) + 0.5 + 1.0 + 2.5;

    // Add the row to the dataframe
    df.add_row(r);
  }

  // Filter the data frame for all of the rows that sum up a value greater
  // than 1000 * 100
  SummationGreaterThanRower rower(1000 * 100);
  df.pmap(rower);

  // Verify that the rower summation is correct
  ASSERT_EQ(rower.get_summation(), expected_summation);

  exit(0);
}

void test_euclidean() {
  Schema s("IIII");

  // Create a very large dataframe
  DataFrame df(s);
  Row  r(df.get_schema());

  // Create 4 vectors
  r.set(0, (int)1);
  r.set(1, (int)1);
  r.set(2, (int)1);
  r.set(3, (int)1);
  df.add_row(r);
  r.set(0, (int)0);
  r.set(1, (int)2);
  r.set(2, (int)1);
  r.set(3, (int)2);
  df.add_row(r);
  r.set(0, (int)2);
  r.set(1, (int)2);
  r.set(2, (int)2);
  r.set(3, (int)2);
  df.add_row(r);
  r.set(0, (int)4);
  r.set(1, (int)0);
  r.set(2, (int)0);
  r.set(3, (int)3);
  df.add_row(r);

  // Filter the data frame for all of the rows that sum up a value greater
  // than 1000 * 100
  EuclideanMagnitudeRower rower(df.nrows());
  df.map(rower);

  // Verify that the rower has gotten the correct results
  ASSERT_EQ(rower.get_num_vectors(), df.nrows());
  ASSERT_EQ(rower.get_euclidean_magnitude(0), 2.0);
  ASSERT_EQ(rower.get_euclidean_magnitude(1), 3.0);
  ASSERT_EQ(rower.get_euclidean_magnitude(2), 4.0);
  ASSERT_EQ(rower.get_euclidean_magnitude(3), 5.0);
  ASSERT_EQ(rower.get_average(), 3.5);

  exit(0);
}

void test_euclidean_pmap() {
  Schema s("IIII");

  // Create a very large dataframe
  DataFrame df(s);
  Row  r(df.get_schema());

  // Create 4 vectors
  r.set(0, (int)1);
  r.set(1, (int)1);
  r.set(2, (int)1);
  r.set(3, (int)1);
  df.add_row(r);
  r.set(0, (int)0);
  r.set(1, (int)2);
  r.set(2, (int)1);
  r.set(3, (int)2);
  df.add_row(r);
  r.set(0, (int)2);
  r.set(1, (int)2);
  r.set(2, (int)2);
  r.set(3, (int)2);
  df.add_row(r);
  r.set(0, (int)4);
  r.set(1, (int)0);
  r.set(2, (int)0);
  r.set(3, (int)3);
  df.add_row(r);

  // Filter the data frame for all of the rows that sum up a value greater
  // than 1000 * 100
  EuclideanMagnitudeRower rower(df.nrows());
  df.pmap(rower);

  // Verify that the rower has gotten the correct results
  ASSERT_EQ(rower.get_num_vectors(), df.nrows());
  ASSERT_EQ(rower.get_euclidean_magnitude(0), 2.0);
  ASSERT_EQ(rower.get_euclidean_magnitude(1), 3.0);
  ASSERT_EQ(rower.get_euclidean_magnitude(2), 4.0);
  ASSERT_EQ(rower.get_euclidean_magnitude(3), 5.0);
  ASSERT_EQ(rower.get_average(), 3.5);

  exit(0);
}

TEST(A5, test_bool_accept_case) { ASSERT_EXIT_ONE(test_bool_accept_case); }
TEST(A5, test_str_accept_case) { ASSERT_EXIT_ONE(test_str_accept_case); }
TEST(A5, test_float_accept_case) { ASSERT_EXIT_ZERO(test_float_accept_case); }
TEST(A5, test_int_accept_case) { ASSERT_EXIT_ZERO(test_int_accept_case); }
TEST(A5, basic){ ASSERT_EXIT_ZERO(basic); }
TEST(A5, test1) { ASSERT_EXIT_ZERO(test1); }
TEST(A5, test2) { ASSERT_EXIT_ZERO(test2); }
TEST(A5, test3) { ASSERT_EXIT_ZERO(test3); }
TEST(A5, test4) { ASSERT_EXIT_ZERO(test4); }
TEST(A5, test6) { ASSERT_EXIT_ZERO(test6); }
TEST(A5, test7) { ASSERT_EXIT_ZERO(test7); }
TEST(A5, test_pmap) { ASSERT_EXIT_ZERO(test_pmap); }
TEST(A5, test_euclidean) { ASSERT_EXIT_ZERO(test_euclidean); }
TEST(A5, test_euclidean_pmap) { ASSERT_EXIT_ZERO(test_euclidean_pmap); }

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

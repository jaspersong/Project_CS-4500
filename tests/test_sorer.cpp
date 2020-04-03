// lang:Cpp

#include "dataframe.h"
#include "dataframe_column.h"
#include "helper.h"
#include "sorer_integrator.h"

Sys helper;

void test_default() {
  String hi("hi");

  SorerIntegrator integrator("../data/default.sor", 0, 100);
  integrator.parse();
  DataFrame *df = integrator.convert();

  helper.t_true(df->get_schema().col_type(0) == ColumnType_Bool);
  helper.t_true(df->get_schema().col_type(1) == ColumnType_Integer);
  helper.t_true(df->get_schema().col_type(2) == ColumnType_String);
  helper.t_true(df->get_string(2, 0)->equals(&hi));
  helper.t_true(df->get_int(1, 0) == 23);
  helper.t_false(df->get_bool(0, 0));

  delete df;

  helper.OK("Test test_default passed");
}

void test_testfile() {
  String hello("hello");
  String ten("10");

  SorerIntegrator integrator("../data/testfile.sor", 0);
  integrator.parse();
  DataFrame *df = integrator.convert();

  helper.t_true(df->get_schema().col_type(0) == ColumnType_Integer);
  helper.t_true(df->get_schema().col_type(1) == ColumnType_Float);
  helper.t_true(df->get_schema().col_type(2) == ColumnType_String);
  helper.t_true(df->get_schema().col_type(3) == ColumnType_Bool);
  helper.t_true(df->get_int(0, 2) == 9);
  helper.t_true(df->get_float(1, 2) == 6.6f);
  helper.t_true(df->get_float(1, 1) == 10.0f);
  helper.t_true(df->get_string(2, 0)->equals(&hello));
  helper.t_true(df->get_string(2, 2)->equals(&ten));

  delete df;

  helper.OK("Test test_testfile passed");
}

void test_spaces() {
  String bye(" bye ");

  SorerIntegrator integrator("../data/spacey.sor");
  integrator.parse();
  DataFrame *df = integrator.convert();

  helper.t_true(df->get_string(3, 0)->equals(&bye));
  helper.t_true(df->get_float(2, 0) == 2.2f);
  helper.t_true(df->get_schema().col_type(2) == ColumnType_Float);

  delete df;

  helper.OK("Test test_spaces passed");
}

int main() {
  test_default();
  test_testfile();
  test_spaces();
}

// lang:cwc

#include "helper.h"
#include "SorerIntegrator.h"
#include "dataframe_column.h"
#include "dataframe.h"

Sys helper;

void test1() {
  SorerIntegrator integrator("../data/default.sor", 0, 100);
  integrator.parse();
  DataFrame *df = integrator.convert();
  helper.t_true(df->get_schema().col_type(0) == ColumnType_Bool);

  delete df;
}

int main() {
  test1();
}

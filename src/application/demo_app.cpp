/**
 * Name: Snowy Chen, Joe Song
 * Date: 21 March 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::Cpp

#include "demo_app.h"

Demo::Demo() : Application(3) {}

void Demo::main() {
  switch (this->get_node_id()) {
  case 0:
    this->producer();
    break;
  case 1:
    this->counter();
    break;
  case 2:
  default:
    this->summarizer();
    break;
  }
}

void Demo::producer() {
  size_t SZ = 100 * 1000;
  auto *vals = new float[SZ];
  float sum = 0;

  // Create the array
  for (size_t i = 0; i < SZ; ++i) {
    vals[i] = i;
    sum += i;
  }

  // Create the dataframes to the keys.
  KeyValueStore::from_array(this->main_key, this->kv, SZ, vals);
  KeyValueStore::from_scalar(this->check, this->kv, sum);

  delete[] vals;
}

void Demo::counter() {
  Sys helper;

  // Get the dataframe
  DataFrame *v = this->kv->wait_and_get(this->main_key);

  // Sum up the values from the main dataframe
  float sum = 0.0f;
  for (size_t i = 0; i < 100 * 1000; ++i) {
    sum += v->get_float(0, i);
  }

  // Output the result
  helper.p("The sum is  ").pln(sum);

  // Store it into the verify key.
  KeyValueStore::from_scalar(this->verify, this->kv, sum);
  DataFrame *counter_df = this->kv->wait_and_get(this->verify);

  delete v;
  delete counter_df;
}

void Demo::summarizer() {
  Sys helper;

  // Get the result and the verify dataframes.
  DataFrame *result = this->kv->wait_and_get(this->verify);
  DataFrame *expected = this->kv->wait_and_get(this->check);

  // Check to make sure that the results are the same.
  helper.pln(expected->get_float(0, 0) == result->get_float(0, 0) ? "SUCCESS"
                                                                  : "FAILURE");

  delete result;
  delete expected;
}

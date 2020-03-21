/**
 * Name: Snowy Chen, Joe Song
 * Date: 21 February 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::CwC

#pragma once

#include "application.h"
#include "dataframe.h"
#include "helper.h"

class Demo : public Application {
public:
  Key main("main", 0);
  Key verify("verif", 0);
  Key check("ck", 0);
 
  /**
   * Constructs the demo application 
   */
  Demo(size_t node_id) : Application(node_id) {}
 
  void run() override {
    switch(this->get_node_id()) {
    case 0:
      producer();
      break;
    case 1:
      counter();
      break;
    case 2:
    default:
      summarizer();
      break;
   }
  }
 
  void producer() {
    size_t SZ = 100*1000;
    float* vals = new float[SZ];
    float sum = 0;

    // Create the array
    for (size_t i = 0; i < SZ; ++i) {
      vals[i] = i;
      sum += i;
    }

    // Create the dataframes to the keys.
    KeyValueStore::fromArray(&main, this->kv, SZ, vals);
    KeyValueStore::fromScalar(&check, this->kv, sum);
  }
 
  void counter() {
    Sys helper;

    // Get the dataframe
    DataFrame* v = this->kv->waitAndGet(main);

    // Sum up the values from the main dataframe
    float sum = 0.0f;
    for (size_t i = 0; i < 100*1000; ++i) {
      sum += v->get_float(0, i);
    }

    // Output the result
    helper.p("The sum is  ").pln(sum);

    // Store it into the verify key.
    KeyValueStore::fromScalar(&verify, this->kv, sum);

    delete v;
  }
 
  void summarizer() {
    Sys helper;

    // Get the result and the verify dataframes.
    DataFrame* result = this->kv->waitAndGet(verify);
    DataFrame* expected = this->kv->waitAndGet(check);

    // Check to make sure that the results are the same.
    helper.pln(expected->get_float(0,0) == result->get_float(0,0) ? "SUCCESS" : "FAILURE");

    delete result;
    delete expected;
  }
};

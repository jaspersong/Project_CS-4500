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

/**
 * A trivial implementation of an application that could be run on the eau2
 * system.
 */
class Trivial : public Application {
public:
  void run() override {
    size_t SZ = 1000 * 1000;
    float *vals = new float[SZ];

    double sum = 0;
    for (size_t i = 0; i < SZ; ++i) {
      vals[i] = static_cast<float>(i);
      sum += i;
    }

    Key key("triv", 0);
    DataFrame *df = KeyValueStore::fromArray(&key, this->kv, SZ, vals);
    assert(df->get_float(0, 1) == 1);

    DataFrame *df2 = reinterpret_cast<DataFrame *>(this->kv.get(&key));
    for (size_t i = 0; i < SZ; ++i)
      sum -= df2->get_float(0, i);
    assert(sum == 0);

    delete df;
  }
};

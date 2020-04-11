/**
 * Name: Snowy Chen, Joe Song
 * Date: 21 March 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::Cpp

#include "trivial_app.h"
#include "key_value_store.h"

void Trivial::main() {
  size_t SZ = 1000 * 1000;
  auto *vals = new float[SZ];

  double sum = 0;
  for (size_t i = 0; i < SZ; ++i) {
    vals[i] = static_cast<float>(i);
    sum += i;
  }

  Key key("triv");
  KeyValueStore::from_array(key, this->kv, SZ, vals);
  DistributedValue *df = this->kv->wait_and_get(key);

  assert(df->get_float(0, 1) == 1);
  for (size_t i = 0; i < SZ; ++i) {
    sum -= df->get_float(0, i);
  }
  assert(sum == 0);

  delete[] vals;
}

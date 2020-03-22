/**
 * Name: Snowy Chen, Joe Song
 * Date: 21 March 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::Cpp

#include "print_rower.h"

PrintFielder::PrintFielder(Row &row) {
  this->row = &row; }

void PrintFielder::start(size_t r) {
  // Visit each column within the row provided to the fielder.
  for (size_t c = 0; c < this->row->width(); c++) {
    this->row->visit(c, *this);
  }
}

void PrintFielder::accept(bool b) { printf("<%d> ", b); }

void PrintFielder::accept(float f) { printf("<%f> ", f); }

void PrintFielder::accept(int i) { printf("<%d> ", i); }

void PrintFielder::accept(String *s) {
  if (s != nullptr) {
    printf("<%s> ", s->c_str());
  } else {
    printf("<> ");
  }
}

bool PrintRower::accept(Row &r) {
  // Use a fielder to print out this row.
  auto *fielder = new PrintFielder(r);
  fielder->start(r.get_idx());
  fielder->done();

  // Go to the next line in the stdout.
  printf("\n");
  return true;
}

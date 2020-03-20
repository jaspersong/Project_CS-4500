// Lang::CwC

#include "custom_object.h"
#include "custom_string.h"
#include "queue.h"

// clang-format off
void FAIL() { exit(1); }
void OK(const char* m) { /** print m */ }
void t_true(bool p) { if (!p) FAIL(); }
void t_false(bool p) { if (p) FAIL(); }
// clang-format on

void test1() {
  // Basic test
  Queue *q = new Queue(10);

  t_true(q->getSize() == 0);
  t_true(q->isEmpty());

  String *s = new String("Hello");
  String *t = new String("World");

  q->enqueue(s);
  q->enqueue(t);
  CustomObject *s1 = q->getBottom();

  t_true(s->equals(s1));

  delete q;
  delete s;
  delete t;

  OK("1");
}

void test2() {
  // Test getTop and getBottom and getSize
  Queue *q = new Queue(10);

  CustomObject *s = new CustomObject();
  CustomObject *t = new CustomObject();

  q->enqueue(s);
  q->enqueue(t);
  CustomObject *s1 = q->getBottom();

  t_true(s->equals(s1));

  t_true(t->equals(q->getTop()));

  t_true(q->getSize() == 2);

  delete q;
  delete s;
  delete t;

  OK("2");
}

void test3() {
  // Test enqueue and dequeue
  Queue *q = new Queue(10);

  String *s = new String("Hello");
  String *t = new String("World");

  q->enqueue(s);
  q->enqueue(t);
  q->dequeue();

  t_false(q->isEmpty());

  q->dequeue();

  t_true(q->isEmpty());

  delete q;
  delete s;
  delete t;

  OK("3");
}

void test4() {
  // Test contains
  Queue *q = new Queue(10);

  String *s = new String("Hello");
  String *t = new String("World");
  String *u = new String("CS4500");

  q->enqueue(s);
  q->enqueue(t);

  t_true(q->contains(t));
  t_false(q->contains(u));

  delete q;
  delete s;
  delete t;
  delete u;

  OK("4");
}

void test5() {
  // Test equals and hash
  Queue *q = new Queue(10);

  String *s = new String("Hello");
  String *t = new String("World");
  String *u = new String("CS4500");

  q->enqueue(s);
  q->enqueue(t);

  Queue *q2 = new Queue(10);

  String *s2 = new String("Hello");
  String *t2 = new String("World");

  q2->enqueue(s2);
  q2->enqueue(t2);

  t_true(q->equals(q2));
  t_true(q->hash() == q2->hash());

  q->enqueue(u);
  t_false(q->equals(q2));

  delete q;
  delete s;
  delete t;
  delete u;
  delete q2;
  delete s2;
  delete t2;
  OK("5");
}

void test6() {
  // Test adding tons of items
  Queue *q = new Queue(10);

  String *s = new String("Hello");

  for (unsigned long i = 0; i < 1000; i++) {
    q->enqueue(s);
  }

  t_true(q->getSize() == 1000);

  for (unsigned long i = 0; i < 1000; i++) {
    q->dequeue();
  }

  t_true(q->getSize() == 0);
  t_true(q->isEmpty());
  t_true(q->dequeue() == nullptr);

  delete q;
  delete s;

  OK("6");
}

int main(int argc, char *argv[]) {
  test1();
  test2();
  test3();
  test4();
  test5();
  test6();

  return 0;
}

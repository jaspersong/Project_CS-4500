// Lang::CwC

#include "custom_object.h"
#include "custom_string.h"
#include "helper.h"
#include "queue.h"

Sys helper;

void test1() {
  // Basic test
  Queue *q = new Queue(10);

  helper.t_true(q->getSize() == 0);
  helper.t_true(q->isEmpty());

  String *s = new String("Hello");
  String *t = new String("World");

  q->enqueue(s);
  q->enqueue(t);
  CustomObject *s1 = q->getBottom();

  helper.t_true(s->equals(s1));

  delete q;
  delete s;
  delete t;

  helper.OK("Test 1 passed");
}

void test2() {
  // Test getTop and getBottom and getSize
  Queue *q = new Queue(10);

  CustomObject *s = new CustomObject();
  CustomObject *t = new CustomObject();

  q->enqueue(s);
  q->enqueue(t);
  CustomObject *s1 = q->getBottom();

  helper.t_true(s->equals(s1));

  helper.t_true(t->equals(q->getTop()));

  helper.t_true(q->getSize() == 2);

  delete q;
  delete s;
  delete t;

  helper.OK("Test 2 passed");
}

void test3() {
  // Test enqueue and dequeue
  Queue *q = new Queue(10);

  String *s = new String("Hello");
  String *t = new String("World");

  q->enqueue(s);
  q->enqueue(t);
  q->dequeue();

  helper.t_false(q->isEmpty());

  q->dequeue();

  helper.t_true(q->isEmpty());

  delete q;
  delete s;
  delete t;

  helper.OK("Test 3 passed");
}

void test4() {
  // Test contains
  Queue *q = new Queue(10);

  String *s = new String("Hello");
  String *t = new String("World");
  String *u = new String("CS4500");

  q->enqueue(s);
  q->enqueue(t);

  helper.t_true(q->contains(t));
  helper.t_false(q->contains(u));

  delete q;
  delete s;
  delete t;
  delete u;

  helper.OK("Test 4 passed");
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

  helper.t_true(q->equals(q2));
  helper.t_true(q->hash() == q2->hash());

  q->enqueue(u);
  helper.t_false(q->equals(q2));

  delete q;
  delete s;
  delete t;
  delete u;
  delete q2;
  delete s2;
  delete t2;

  helper.OK("Test 5 passed");
}

void test6() {
  // Test adding tons of items
  Queue *q = new Queue(10);

  String *s = new String("Hello");

  for (unsigned long i = 0; i < 1000; i++) {
    q->enqueue(s);
  }

  helper.t_true(q->getSize() == 1000);

  for (unsigned long i = 0; i < 1000; i++) {
    q->dequeue();
  }

  helper.t_true(q->getSize() == 0);
  helper.t_true(q->isEmpty());
  helper.t_true(q->dequeue() == nullptr);

  delete q;
  delete s;

  helper.OK("Test 6 passed");
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

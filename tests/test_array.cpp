// lang:cwc

#include "array.h"
#include "custom_object.h"
#include "custom_string.h"
#include <stdio.h>

// will be called when a test fails
void fail() { perror("Test failure: program terminating"); exit(1); }

void pass() { printf("Test passed!"); }

void checkTrue(bool b) {
  if (!b)
    fail();
}

void checkFalse(bool b) {
  if (b)
    fail();
}

void objectArrayTest() {
  CustomObject *o1 = new CustomObject();
  CustomObject *o2 = new CustomObject();
  CustomObject *o3 = new CustomObject();
  CustomObject *o4 = new CustomObject();

  Array *arr1 = new Array();

  checkTrue(arr1->getSize() == 0);
  checkTrue(arr1->indexOf(o1) == -1);

  // insert elements
  arr1->insertAtEnd(o1);
  arr1->insertAtEnd(o2);

  // inserting in-between o1 and o2
  arr1->insert(o3, 1);

  // inserting at end differently
  arr1->insert(o4, 3);

  checkTrue(arr1->getSize() == 4);
  checkTrue(arr1->getElementAt(0)->equals(o1));
  checkTrue(arr1->indexOf(o1) == 0);
  checkTrue(arr1->getElementAt(1)->equals(o3));
  checkTrue(arr1->indexOf(o3) == 1);
  checkTrue(arr1->getElementAt(2)->equals(o2));
  checkTrue(arr1->indexOf(o2) == 2);
  checkTrue(arr1->getElementAt(3)->equals(o4));
  checkTrue(arr1->indexOf(o4) == 3);

  // remove middle element and check return
  checkTrue(arr1->remove(1)->equals(o3));

  checkTrue(arr1->getSize() == 3);
  checkTrue(arr1->getElementAt(0)->equals(o1));
  checkTrue(arr1->getElementAt(1)->equals(o2));
  checkTrue(arr1->getElementAt(2)->equals(o4));

  checkTrue(arr1->indexOf(o1) == 0);
  checkTrue(arr1->indexOf(o2) == 1);
  checkTrue(arr1->indexOf(o3) == -1);
  checkTrue(arr1->indexOf(o4) == 2);

  // Create new array
  Array *arr2 = new Array();
  CustomObject *o5 = new CustomObject();
  CustomObject *o6 = new CustomObject();

  arr2->insertAtEnd(o5);
  arr2->insert(o6, 0);

  checkTrue(arr2->getSize() == 2);
  checkTrue(arr2->getElementAt(0)->equals(o6));
  checkTrue(arr2->getElementAt(1)->equals(o5));

  // insert arr1 into arr2
  arr2->insertMultiple(arr1, 1);

  checkTrue(arr2->getSize() == 5);
  checkTrue(arr2->getElementAt(0)->equals(o6));
  checkTrue(arr2->getElementAt(1)->equals(o1));
  checkTrue(arr2->getElementAt(2)->equals(o2));
  checkTrue(arr1->indexOf(o2) == 1); // wrong one
  checkTrue(arr2->getElementAt(3)->equals(o4));
  checkTrue(arr2->getElementAt(4)->equals(o5));

  checkTrue(arr2->set(o1, 2)->equals(o2));
  checkTrue(arr2->getSize() == 5);
  checkTrue(arr2->getElementAt(0)->equals(o6));
  checkTrue(arr2->getElementAt(1)->equals(o1));
  checkTrue(arr2->indexOf(o1) == 1); // wrong one

  arr2->remove(0);
  checkTrue(arr2->getSize() == 4);
  checkTrue(arr2->indexOf(o6) == -1);
  checkTrue(arr2->getElementAt(0)->equals(o1));
  checkTrue(arr2->getElementAt(1)->equals(o1));
  checkTrue(arr2->getElementAt(2)->equals(o4));

  arr2->remove(1);
  checkTrue(arr2->getSize() == 3);
  checkTrue(arr2->getElementAt(0)->equals(o1));
  checkTrue(arr2->getElementAt(1)->equals(o4));
  checkTrue(arr2->getElementAt(2)->equals(o5));

  arr2->clear();
  checkTrue(arr2->getSize() == 0);
  arr2->insertAtEnd(o3);
  checkTrue(arr2->getSize() == 1);
  checkTrue(arr2->getElementAt(0)->equals(o3));

  delete arr1;
  delete arr2;
}

void stringArrayTest() {
  String *s1 = new String("abc");
  String *s2 = new String("Hello World");
  char *cArr1 = new char[6];
  cArr1[0] = 'H';
  cArr1[1] = 'e';
  cArr1[2] = 'l';
  cArr1[3] = 'l';
  cArr1[4] = '0';
  cArr1[5] = 0;
  String *s3 = new String(cArr1);

  StringArray *strArr = new StringArray();
  checkTrue(strArr->getSize() == 0);
  checkTrue(strArr->indexOf(s1) == -1);

  strArr->insert(s2, 0);
  strArr->insert(s1, 0);
  strArr->insertAtEnd(s3);
  checkTrue(strArr->getSize() == 3);
  checkTrue(strArr->getElementAt(0)->equals(s1));
  checkTrue(strArr->getElementAt(1)->equals(s2));
  checkTrue(strArr->getElementAt(2)->equals(s3));

  String *s4 = new String("start of arr2");
  String *s5 = new String("hippo");
  StringArray *strArr2 = new StringArray();
  strArr2->insertAtEnd(s4);
  strArr2->insertAtEnd(s5);

  checkTrue(strArr2->getSize() == 2);
  checkTrue(strArr->indexOf(s2) == 1);
  strArr->insertMultiple(strArr2, 1);
  checkTrue(strArr->indexOf(s4) == 1);
  checkTrue(strArr->indexOf(s2) == 3);

  checkTrue(strArr->getSize() == 5);
  checkTrue(strArr->getElementAt(1)->equals(s4));
  String *sRemoved = strArr->remove(1);
  checkTrue(sRemoved->equals(s4));
  checkTrue(strArr->getSize() == 4);
  checkTrue(strArr->getElementAt(1)->equals(s5));

  delete strArr;
  delete strArr2;
}

void intArrayTest() {
  IntArray *arr1 = new IntArray();
  checkTrue(arr1->getSize() == 0);

  int a = 32;
  int b = -7;
  int c = 0;

  // Does not exist
  checkTrue(arr1->indexOf(b) == -1);

  arr1->insert(a, 0);
  arr1->insert(b, 0);
  arr1->insert(c, 2);

  checkTrue(arr1->getSize() == 3);
  checkTrue(arr1->getElementAt(0) == b);
  checkTrue(arr1->getElementAt(1) == a);
  checkTrue(arr1->getElementAt(2) == c);

  int d = 4;
  int e = 5;

  arr1->insertAtEnd(d);
  checkTrue(arr1->getSize() == 4);
  checkTrue(arr1->getElementAt(3) == d);
  checkTrue(arr1->indexOf(c) == 2);

  IntArray *arr2 = new IntArray();
  arr2->insertAtEnd(e);
  arr2->insertMultiple(arr1, 0);
  checkTrue(arr2->getSize() == 5);
  checkTrue(arr2->getElementAt(0) == b);
  checkTrue(arr2->getElementAt(4) == e);
  checkTrue(arr2->getElementAt(2) == c);

  arr1->insert(e, 4);
  checkTrue(arr1->equals(arr2));

  checkTrue(arr1->remove(3) == d);
  checkTrue(arr1->getSize() == 4);

  int f = 77;
  checkTrue(arr1->set(f, 2) == c);
  checkTrue(arr1->getElementAt(2) == f);

  arr1->clear();
  checkTrue(arr1->getSize() == 0);
}

void floatArrayTest() {
  FloatArray *arr1 = new FloatArray();
  checkTrue(arr1->getSize() == 0);

  float a = -2;
  float b = 6;

  // Does not exist
  checkTrue(arr1->indexOf(b) == -1);

  arr1->insert(a, 0);
  arr1->insert(b, 0);
  arr1->insert(a, 1);

  checkTrue(arr1->getSize() == 3);
  checkTrue(arr1->getElementAt(0) == b);
  checkTrue(arr1->getElementAt(1) == a);
  checkTrue(arr1->getElementAt(2) == a);

  checkTrue(arr1->remove(1) == a);
  checkTrue(arr1->getSize() == 2);

  float c = 90.7;
  checkTrue(arr1->set(c, 1) == a);
  checkTrue((arr1->getElementAt(1) > 90.7 - 0.001)
  && (arr1->getElementAt(1) < 90.7 + 0.001)); // Accuracy check

  float d = 40.4;
  arr1->insertAtEnd(d);
  checkTrue(arr1->getSize() == 3);
  checkTrue(arr1->getElementAt(2) == d);
  checkTrue(arr1->indexOf(c) == 1);

  FloatArray *arr2 = new FloatArray();
  arr2->insertMultiple(arr1, 0);
  checkTrue(arr1->equals(arr2));

  checkTrue(arr1->remove(1) == c);
  checkTrue(arr1->getSize() == 2);

  arr1->clear();
  checkTrue(arr1->getSize() == 0);
}

void boolArrayTest() {
  BoolArray *boolArr1 = new BoolArray();
  bool b1 = true;
  bool b2 = false;
  bool b3 = true;

  boolArr1->insert(b1, 0);
  boolArr1->insertAtEnd(b2);
  boolArr1->insert(b3, 0);

  checkTrue(boolArr1->getSize() == 3);
  checkTrue(boolArr1->getElementAt(0) == b3);
  checkTrue(boolArr1->getElementAt(1) == b1);
  checkTrue(boolArr1->getElementAt(2) == b2);
  checkTrue(boolArr1->indexOf(true) == 0);
  checkTrue(boolArr1->indexOf(false) == 2);

  BoolArray *boolArr2 = new BoolArray();
  boolArr2->insertMultiple(boolArr1, 0);
  checkTrue(boolArr1->equals(boolArr2));

  boolArr2->insertAtEnd(false);
  checkTrue(boolArr2->getSize() == 4);
  checkFalse(boolArr1->equals(boolArr2));

  checkFalse(boolArr2->set(true, 2));
  checkTrue(boolArr2->getElementAt(2));

  checkTrue(boolArr2->remove(2));
  checkFalse(boolArr2->remove(2));

  checkTrue(boolArr2->getSize() == 2);
  checkTrue(boolArr2->getElementAt(0));
  checkTrue(boolArr2->getElementAt(1));

  boolArr2->clear();
  checkTrue(boolArr2->getSize() == 0);
}

int main() {
  objectArrayTest();
  stringArrayTest();
  intArrayTest();
  floatArrayTest();
  boolArrayTest();
}
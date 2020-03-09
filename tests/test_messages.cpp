/**
 * Name: Snowy Chen, Joe Song
 * Date: 28 February 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// lang::Cpp

#include <gtest/gtest.h>

#include "serial.h"

#define ASSERT_T(a) ASSERT_EQ((a), true)
#define ASSERT_F(a) ASSERT_EQ((a), false)
#define ASSERT_EXIT_ZERO(a) ASSERT_EXIT(a(), ::testing::ExitedWithCode(0), ".*")

// TODO: Convert main.cpp from A6Part2 into google test suite

void test_size_t() {
  Status *dummy = new Status();
  dummy->set_sender_id(1);
  dummy->set_target_id(2);
  dummy->set_id(3);

  size_t offset = 0;
  size_t expected_buffer_size = sizeof(MsgKind) + sizeof(size_t) * 4;
  unsigned char *buffer = dummy->prepare_serialize_buffer(0, offset);
  ASSERT_EQ(offset, expected_buffer_size);
  ASSERT_F(buffer == nullptr);

  offset = sizeof(MsgKind);
  ASSERT_EQ(
      Message::get_size_t_deserialization(buffer, offset, expected_buffer_size),
      1);
  ASSERT_EQ(offset, sizeof(MsgKind) + sizeof(size_t));
  ASSERT_EQ(
      Message::get_size_t_deserialization(buffer, offset, expected_buffer_size),
      2);
  ASSERT_EQ(offset, sizeof(MsgKind) + sizeof(size_t) + sizeof(size_t));
  ASSERT_EQ(
      Message::get_size_t_deserialization(buffer, offset, expected_buffer_size),
      3);
  ASSERT_EQ(offset, sizeof(MsgKind) + 3 * sizeof(size_t));

  offset = 0;
  ASSERT_EQ(Message::set_size_t_serialization(5, buffer, offset,
                                              expected_buffer_size),
            sizeof(size_t));
  offset = 0;
  ASSERT_EQ(
      Message::get_size_t_deserialization(buffer, offset, expected_buffer_size),
      5);

  delete dummy;
  delete buffer;

  exit(0);
}

void test_string() {
  Status *dummy = new Status();
  dummy->set_sender_id(1);
  dummy->set_target_id(2);
  dummy->set_id(3);
  dummy->set_payload_size(50);


  size_t offset = 0;
  size_t expected_buffer_size = sizeof(MsgKind) + sizeof(size_t) * 4 + 50;
  unsigned char *buffer = dummy->prepare_serialize_buffer(50, offset);
  ASSERT_F(buffer == nullptr);

  offset = 0;
  String *test_string = new String("hello");
  ASSERT_EQ(Message::set_string_serialization(test_string, buffer, offset,
                                              expected_buffer_size),
            sizeof(size_t) + sizeof(char) * 6);
  offset = 0;
  String *result_string = Message::get_string_deserialization(buffer, offset,
                                                              expected_buffer_size);
  ASSERT_T(test_string->equals(result_string));

  delete dummy;
  delete buffer;
  delete test_string;
  delete result_string;

  exit(0);
}
void test_int() {
  Status *dummy = new Status();
  dummy->set_sender_id(1);
  dummy->set_target_id(2);
  dummy->set_id(3);

  size_t offset = 0;
  size_t expected_buffer_size = sizeof(MsgKind) + sizeof(size_t) * 4;
  unsigned char *buffer = dummy->prepare_serialize_buffer(0, offset);
  ASSERT_EQ(offset, expected_buffer_size);
  ASSERT_F(buffer == nullptr);


  offset = 0;
  ASSERT_EQ(Message::set_int_serialization(5, buffer, offset,
                                           expected_buffer_size),
            sizeof(int));
  offset = 0;
  ASSERT_EQ(
      Message::get_int_deserialization(buffer, offset, expected_buffer_size),
      5);

  delete dummy;
  delete buffer;

  exit(0);
}


TEST(A5, test_int) { ASSERT_EXIT_ZERO(test_int); }
TEST(A5, test_string) { ASSERT_EXIT_ZERO(test_string); }
TEST(A5, test_size_t) { ASSERT_EXIT_ZERO(test_size_t); }

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
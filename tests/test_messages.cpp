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

void test_bool() {
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
  ASSERT_EQ(Message::set_bool_serialization(true, buffer, offset,
                                           expected_buffer_size),
            sizeof(bool));
  offset = 0;
  ASSERT_EQ(
      Message::get_bool_deserialization(buffer, offset, expected_buffer_size),
      true);

  delete dummy;
  delete buffer;

  exit(0);
}

void status_round_trip() {
  String *status_msg = new String("The fire nation attacked.\n");
  Status *status0 = new Status(*status_msg);
  status0->set_target_id(1);
  status0->set_sender_id(2);
  status0->set_id(3);

  // Serialize the message
  size_t buffer_size = 0;
  unsigned char *serialized_msg = status0->serialize(buffer_size);

  // Now deserialize it and make sure that all of the values from the
  // original has been retained
  Message *received_msg = Message::deserialize(serialized_msg, buffer_size);

  // Validate the values
  Status *status_received_msg = received_msg->as_status();
  ASSERT_T(status_received_msg != nullptr);
  ASSERT_T(status_received_msg->get_target_id() == 1);
  ASSERT_F(status_received_msg->get_target_id() == 0);
  ASSERT_T(status_received_msg->get_sender_id() == 2);
  ASSERT_F(status_received_msg->get_sender_id() == 0);
  ASSERT_T(status_received_msg->get_id() == 3);
  ASSERT_F(status_received_msg->get_id() == 0);
  String *received_status = status_received_msg->get_message();
  ASSERT_T(received_status->equals(status_msg));

  // Free the memory as appropriate
  delete status_msg;
  delete status0;
  delete[] serialized_msg;
  delete received_msg;
  delete received_status;

  exit(0);
}

void status_funcs() {
  String *status_msg0 = new String("The fire nation attacked.\n");
  String *status_msg1 = new String(
      "Help I've fallen but don't want to get up.\n");
  Status *status0 = new Status(*status_msg0);

  String *recv_msg0 = status0->get_message();
  ASSERT_T(recv_msg0->equals(status_msg0));

  status0->set_message(*status_msg1);
  String *recv_msg1 = status0->get_message();
  ASSERT_T(recv_msg1->equals(status_msg1));

  // Free memory as appropriate
  delete status_msg0;
  delete status_msg1;
  delete status0;
  delete recv_msg0;
  delete recv_msg1;

 exit(0);
}

// Add ack use case
void ack_case() {
  Ack *ack0 = new Ack();
  ack0->set_target_id(1);
  ack0->set_sender_id(2);
  ack0->set_id(3);

  // Serialize the ack message
  size_t buffer_size = 0;
  unsigned char *serialized_msg = ack0->serialize(buffer_size);
  Message *received_msg = Message::deserialize(serialized_msg, buffer_size);
  // test
  Ack *ack_received_message = received_msg->as_ack();
  ASSERT_T(ack_received_message != nullptr);
  ASSERT_T(ack_received_message->get_target_id() == 1);
  ASSERT_F(ack_received_message->get_target_id() == 0);
  ASSERT_T(ack_received_message->get_sender_id() == 2);
  ASSERT_F(ack_received_message->get_sender_id() == 0);
  ASSERT_T(ack_received_message->get_id() == 3);
  ASSERT_F(ack_received_message->get_id() == 0);


  // Free the memory
  delete ack0;
  delete[] serialized_msg;
  delete received_msg;

  exit(0);

}

void nack_case() {
  Nack *nack0 = new Nack();
  nack0->set_target_id(1);
  nack0->set_sender_id(2);
  nack0->set_id(3);

  // seralize the nack message
  size_t buffer_size = 0;
  unsigned char *serialized_msg = nack0->serialize(buffer_size);
  Message *received_msg = Message::deserialize(serialized_msg, buffer_size);

  // test
  Nack *nack_received_message = received_msg->as_nack();
  ASSERT_T(nack_received_message != nullptr);
  ASSERT_T(nack_received_message->get_target_id() == 1);
  ASSERT_F(nack_received_message->get_target_id() == 0);
  ASSERT_T(nack_received_message->get_sender_id() == 2);
  ASSERT_F(nack_received_message->get_sender_id() == 0);
  ASSERT_T(nack_received_message->get_id() == 3);
  ASSERT_F(nack_received_message->get_id() == 0);

  // Free the memory
  delete nack0;
  delete[] serialized_msg;
  delete received_msg;

  exit(0);
}

void put_case() {
  Put *put0 = new Put();
  put0->set_target_id(1);
  put0->set_sender_id(2);
  put0->set_id(3);

  // serialize the put message
  size_t buffer_size = 0;
  unsigned char *serialized_msg = put0->serialize(buffer_size);
  Message *received_msg = Message::deserialize(serialized_msg, buffer_size);

  // test
  Put *put_received_message = received_msg->as_put();
  ASSERT_T(put_received_message != nullptr);
  ASSERT_T(put_received_message->get_target_id() == 1);
  ASSERT_F(put_received_message->get_target_id() == 0);
  ASSERT_T(put_received_message->get_sender_id() == 2);
  ASSERT_F(put_received_message->get_sender_id() == 0);
  ASSERT_T(put_received_message->get_id() == 3);
  ASSERT_F(put_received_message->get_id() == 0);

  // Free the memory
  delete put0;
  delete[] serialized_msg;
  delete received_msg;

  exit(0);
}

void reply_case() {
  Reply *reply0 = new Reply();
  reply0->set_target_id(1);
  reply0->set_sender_id(2);
  reply0->set_id(3);

  // serialize the reply message
  size_t buffer_size = 0;
  unsigned char *serialized_msg = reply0->serialize(buffer_size);
  Message *received_msg = Message::deserialize(serialized_msg, buffer_size);

  // test
  Reply *reply_received_message = received_msg->as_reply();
  ASSERT_T(reply_received_message != nullptr);
  ASSERT_T(reply_received_message->get_target_id() == 1);
  ASSERT_F(reply_received_message->get_target_id() == 0);
  ASSERT_T(reply_received_message->get_sender_id() == 2);
  ASSERT_F(reply_received_message->get_sender_id() == 0);
  ASSERT_T(reply_received_message->get_id() == 3);
  ASSERT_F(reply_received_message->get_id() == 0);

  // Free the memory
  delete reply0;
  delete received_msg;
  delete[] serialized_msg;

  exit(0);
}

void get_case() {
  Get *get0 = new Get();
  get0->set_target_id(1);
  get0->set_sender_id(2);
  get0->set_id(3);

  // serialize the get message
  size_t buffer_size = 0;
  unsigned char *serialized_msg = get0->serialize(buffer_size);
  Message *received_msg = Message::deserialize(serialized_msg, buffer_size);

  // test
  Get *get_received_msg = received_msg->as_get();
  ASSERT_T(get_received_msg != nullptr);
  ASSERT_T(get_received_msg->get_target_id() == 1);
  ASSERT_F(get_received_msg->get_target_id() == 0);
  ASSERT_T(get_received_msg->get_sender_id() == 2);
  ASSERT_F(get_received_msg->get_sender_id() == 0);
  ASSERT_T(get_received_msg->get_id() == 3);
  ASSERT_F(get_received_msg->get_id() == 0);

  // Free the memory
  delete get0;
  delete received_msg;
  delete[] serialized_msg;

  exit(0);
}

void waitAget_case() {
  WaitAndGet *waitAget0 = new WaitAndGet();
  waitAget0->set_target_id(1);
  waitAget0->set_sender_id(2);
  waitAget0->set_id(3);

  // serialize the WaitAndGet message
  size_t buffer_size = 0;
  unsigned char *serialized_msg = waitAget0->serialize(buffer_size);
  Message *received_msg = Message::deserialize(serialized_msg, buffer_size);

  // test
  WaitAndGet *waitAget_received_msg = received_msg->as_waitandget();
  ASSERT_T(waitAget_received_msg != nullptr);
  ASSERT_T(waitAget_received_msg->get_target_id() == 1);
  ASSERT_F(waitAget_received_msg->get_target_id() == 0);
  ASSERT_T(waitAget_received_msg->get_sender_id() == 2);
  ASSERT_F(waitAget_received_msg->get_sender_id() == 0);
  ASSERT_T(waitAget_received_msg->get_id() == 3);
  ASSERT_F(waitAget_received_msg->get_id() == 0);

  // Free the memory
  delete waitAget0;
  delete received_msg;
  delete[] serialized_msg;

  exit(0);

}

void kill_case() {
  Kill *kill0 = new Kill();
  kill0->set_target_id(1);
  kill0->set_sender_id(2);
  kill0->set_id(3);

  // serialize the Kill message
  size_t buffer_size = 0;
  unsigned char *serialized_msg = kill0->serialize(buffer_size);
  Message *received_msg = Message::deserialize(serialized_msg, buffer_size);

  // test
  Kill *kill_received_msg = received_msg->as_kill();
  ASSERT_T(kill_received_msg != nullptr);
  ASSERT_T(kill_received_msg->get_target_id() == 1);
  ASSERT_F(kill_received_msg->get_target_id() == 0);
  ASSERT_T(kill_received_msg->get_sender_id() == 2);
  ASSERT_F(kill_received_msg->get_sender_id() == 0);
  ASSERT_T(kill_received_msg->get_id() == 3);
  ASSERT_F(kill_received_msg->get_id() == 0);

  // Free the memory
  delete kill0;
  delete received_msg;
  delete[] serialized_msg;

  exit(0);
}

void register_case() {
  Register *register0 = new Register();
  register0->set_target_id(1);
  register0->set_sender_id(2);
  register0->set_id(3);

  // serialize the register message
  size_t buffer_size = 0;
  unsigned char *serialized_msg = register0->serialize(buffer_size);
  Message *received_msg = Message::deserialize(serialized_msg, buffer_size);

  // test
  Register *register_received_msg = received_msg->as_register();
  ASSERT_T(register_received_msg != nullptr);
  ASSERT_T(register_received_msg->get_target_id() == 1);
  ASSERT_F(register_received_msg->get_target_id() == 0);
  ASSERT_T(register_received_msg->get_sender_id() == 2);
  ASSERT_F(register_received_msg->get_sender_id() == 0);
  ASSERT_T(register_received_msg->get_id() == 3);
  ASSERT_F(register_received_msg->get_id() == 0);

  // Free the memory
  delete register0;
  delete received_msg;
  delete[] serialized_msg;

  exit(0);
}

void directory_case() {
  size_t max_clients = 10;
  Directory *directory0 = new Directory(max_clients);
  directory0->set_target_id(1);
  directory0->set_sender_id(2);
  directory0->set_id(3);

  // serialize the directory message
  size_t buffer_size = 0;
  unsigned char *serialized_msg = directory0->serialize(buffer_size);
  Message *received_msg = Message::deserialize(serialized_msg, buffer_size);

  // Test
  Directory *directory_received_message = received_msg->as_directory();
  ASSERT_T(directory_received_message != nullptr);
  ASSERT_T(directory_received_message->get_target_id() == 1);
  ASSERT_F(directory_received_message->get_target_id() == 0);
  ASSERT_T(directory_received_message->get_sender_id() == 2);
  ASSERT_F(directory_received_message->get_sender_id() == 0);
  ASSERT_T(directory_received_message->get_id() == 3);
  ASSERT_F(directory_received_message->get_id() == 0);

  // free the memory
  delete directory0;
  delete received_msg;
  delete[] serialized_msg;

  exit(0);
}



TEST(A5, test_int) { ASSERT_EXIT_ZERO(test_int); }
TEST(A5, test_string) { ASSERT_EXIT_ZERO(test_string); }
TEST(A5, test_size_t) { ASSERT_EXIT_ZERO(test_size_t); }
TEST(A5, test_bool) { ASSERT_EXIT_ZERO(test_bool); }
TEST(A5, status_round_trip) { ASSERT_EXIT_ZERO(status_round_trip); }
TEST(A5, status_funcs) { ASSERT_EXIT_ZERO(status_funcs); }
TEST(A5, ack_case) { ASSERT_EXIT_ZERO(ack_case); }
TEST(A5, nack_case) { ASSERT_EXIT_ZERO(nack_case); }
TEST(A5, puck_case) { ASSERT_EXIT_ZERO(put_case); }
TEST(A5, reply_case) { ASSERT_EXIT_ZERO(reply_case); }
TEST(A5, get_case) { ASSERT_EXIT_ZERO(get_case); }
TEST(A5, waitAget_case) { ASSERT_EXIT_ZERO(waitAget_case); }
TEST(A5, kill_case) { ASSERT_EXIT_ZERO(kill_case); }
TEST(A5, register_case) { ASSERT_EXIT_ZERO(register_case); }
TEST(A5, directory_case) { ASSERT_EXIT_ZERO(directory_case); }




int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
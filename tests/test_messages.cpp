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
  Serializer serializer;

  // Serialize a bunch of size_t values
  serializer.set_size_t(1);
  serializer.set_size_t(2);
  serializer.set_size_t(3);

  unsigned char *buffer = serializer.get_serialized_buffer();
  size_t buffer_size = serializer.get_size_serialized_data();
  ASSERT_T(buffer_size != 0);
  ASSERT_T(buffer != nullptr);

  // Now deserialize
  Deserializer deserializer(buffer, buffer_size);
  ASSERT_T(deserializer.get_num_bytes_left() > 0);
  ASSERT_T(deserializer.has_size_t());
  ASSERT_EQ(deserializer.get_size_t(), 1);

  ASSERT_T(deserializer.get_num_bytes_left() > 0);
  ASSERT_T(deserializer.has_size_t());
  ASSERT_EQ(deserializer.get_size_t(), 2);

  ASSERT_T(deserializer.get_num_bytes_left() > 0);
  ASSERT_T(deserializer.has_size_t());
  ASSERT_EQ(deserializer.get_size_t(), 3);

  ASSERT_T(deserializer.get_num_bytes_left() == 0);
  ASSERT_F(deserializer.has_size_t());

  delete buffer;
  exit(0);
}

void test_string() {
  Serializer serializer;
  String hello("hello");
  String world("world");

  // Serialize the strings
  hello.serialize(serializer);
  world.serialize(serializer);

  unsigned char *buffer = serializer.get_serialized_buffer();
  size_t buffer_size = serializer.get_size_serialized_data();
  ASSERT_T(buffer_size != 0);
  ASSERT_T(buffer != nullptr);

  // Now deserialize
  Deserializer deserializer(buffer, buffer_size);
  ASSERT_T(deserializer.get_num_bytes_left() > 0);
  ASSERT_T(deserializer.has_size_t());
  String *ret_hello = String::deserialize_as_string(deserializer);
  ASSERT_T(ret_hello != nullptr);
  ASSERT_T(ret_hello->equals(&hello));

  ASSERT_T(deserializer.get_num_bytes_left() > 0);
  ASSERT_T(deserializer.has_size_t());
  String *ret_world = String::deserialize_as_string(deserializer);
  ASSERT_T(ret_world != nullptr);
  ASSERT_T(ret_world->equals(&world));

  ASSERT_T(deserializer.get_num_bytes_left() == 0);
  ASSERT_F(deserializer.has_size_t());

  delete buffer;
  delete ret_hello;
  delete ret_world;
  exit(0);
}

void test_int() {
  Serializer serializer;

  // Serialize a bunch of int values
  serializer.set_int(4);
  serializer.set_int(5);
  serializer.set_int(6);

  unsigned char *buffer = serializer.get_serialized_buffer();
  size_t buffer_size = serializer.get_size_serialized_data();
  ASSERT_T(buffer_size != 0);
  ASSERT_T(buffer != nullptr);

  // Now deserialize
  Deserializer deserializer(buffer, buffer_size);
  ASSERT_T(deserializer.get_num_bytes_left() > 0);
  ASSERT_T(deserializer.has_int());
  ASSERT_EQ(deserializer.get_int(), 4);

  ASSERT_T(deserializer.get_num_bytes_left() > 0);
  ASSERT_T(deserializer.has_int());
  ASSERT_EQ(deserializer.get_int(), 5);

  ASSERT_T(deserializer.get_num_bytes_left() > 0);
  ASSERT_T(deserializer.has_int());
  ASSERT_EQ(deserializer.get_int(), 6);

  ASSERT_T(deserializer.get_num_bytes_left() == 0);
  ASSERT_F(deserializer.has_int());

  delete buffer;
  exit(0);
}

void test_bool() {
  Serializer serializer;

  // Serialize a bunch of size_t values
  serializer.set_bool(true);
  serializer.set_int(5);
  serializer.set_bool(false);

  unsigned char *buffer = serializer.get_serialized_buffer();
  size_t buffer_size = serializer.get_size_serialized_data();
  ASSERT_T(buffer_size != 0);
  ASSERT_T(buffer != nullptr);

  // Now deserialize
  Deserializer deserializer(buffer, buffer_size);
  ASSERT_T(deserializer.get_num_bytes_left() > 0);
  ASSERT_T(deserializer.has_bool());
  ASSERT_T(deserializer.get_bool());

  ASSERT_T(deserializer.get_num_bytes_left() > 0);
  ASSERT_T(deserializer.has_int());
  ASSERT_EQ(deserializer.get_int(), 5);

  ASSERT_T(deserializer.get_num_bytes_left() > 0);
  ASSERT_T(deserializer.has_bool());
  ASSERT_F(deserializer.get_bool());

  ASSERT_T(deserializer.get_num_bytes_left() == 0);
  ASSERT_F(deserializer.has_bool());

  delete buffer;
  exit(0);
}

void status_round_trip() {
  String status_msg("The fire nation attacked.\n");
  Status status0(status_msg);
  status0.set_target_id(1);
  status0.set_sender_id(2);
  status0.set_id(3);

  // Serialize the message
  Serializer serializer;
  status0.serialize(serializer);
  unsigned char *buffer = serializer.get_serialized_buffer();
  size_t buffer_size = serializer.get_size_serialized_data();

  // Now deserialize it and make sure that all of the values from the
  // original has been retained
  Message *received_msg = Message::deserialize_as_message(buffer, buffer_size);

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
  ASSERT_T(received_status->equals(&status_msg));

  // Free the memory as appropriate
  delete[] buffer;
  delete received_msg;
  delete received_status;

  exit(0);
}

void status_funcs() {
  String status_msg0("The fire nation attacked.\n");
  String status_msg1("Help I've fallen but don't want to get up.\n");
  Status status0(status_msg0);

  String *recv_msg0 = status0.get_message();
  ASSERT_T(recv_msg0->equals(&status_msg0));

  status0.set_message(status_msg1);
  String *recv_msg1 = status0.get_message();
  ASSERT_T(recv_msg1->equals(&status_msg1));

  // Free memory as appropriate
  delete recv_msg0;
  delete recv_msg1;

 exit(0);
}

// Add ack use case
void ack_case() {
  Ack ack0;
  ack0.set_target_id(1);
  ack0.set_sender_id(2);
  ack0.set_id(3);

  // Serialize the message
  Serializer serializer;
  ack0.serialize(serializer);
  unsigned char *buffer = serializer.get_serialized_buffer();
  size_t buffer_size = serializer.get_size_serialized_data();

  // Now deserialize it and make sure that all of the values from the
  // original has been retained
  Message *received_msg = Message::deserialize_as_message(buffer, buffer_size);

  // test
  Ack *ack_received_message = received_msg->as_ack();
  ASSERT_T(ack_received_message != nullptr);
  ASSERT_EQ(ack_received_message->get_target_id(), 1);
  ASSERT_EQ(ack_received_message->get_sender_id(), 2);
  ASSERT_EQ(ack_received_message->get_id(), 3);

  // Free the memory
  delete[] buffer;
  delete received_msg;

  exit(0);
}

void nack_case() {
  Nack nack0;
  nack0.set_target_id(1);
  nack0.set_sender_id(2);
  nack0.set_id(3);

  // Serialize the message
  Serializer serializer;
  nack0.serialize(serializer);
  unsigned char *buffer = serializer.get_serialized_buffer();
  size_t buffer_size = serializer.get_size_serialized_data();

  // Now deserialize it and make sure that all of the values from the
  // original has been retained
  Message *received_msg = Message::deserialize_as_message(buffer, buffer_size);

  // test
  Nack *nack_received = received_msg->as_nack();
  ASSERT_T(nack_received != nullptr);
  ASSERT_T(nack_received->get_target_id() == 1);
  ASSERT_F(nack_received->get_target_id() == 0);
  ASSERT_T(nack_received->get_sender_id() == 2);
  ASSERT_F(nack_received->get_sender_id() == 0);
  ASSERT_T(nack_received->get_id() == 3);
  ASSERT_F(nack_received->get_id() == 0);

  // Free the memory
  delete[] buffer;
  delete received_msg;

  exit(0);
}

void put_case() {
  Put put0;
  put0.set_target_id(1);
  put0.set_sender_id(2);
  put0.set_id(3);

  // Serialize the message
  Serializer serializer;
  put0.serialize(serializer);
  unsigned char *buffer = serializer.get_serialized_buffer();
  size_t buffer_size = serializer.get_size_serialized_data();

  // Now deserialize it and make sure that all of the values from the
  // original has been retained
  Message *received_msg = Message::deserialize_as_message(buffer, buffer_size);

  // test
  Put *put_received = received_msg->as_put();
  ASSERT_T(put_received != nullptr);
  ASSERT_T(put_received->get_target_id() == 1);
  ASSERT_F(put_received->get_target_id() == 0);
  ASSERT_T(put_received->get_sender_id() == 2);
  ASSERT_F(put_received->get_sender_id() == 0);
  ASSERT_T(put_received->get_id() == 3);
  ASSERT_F(put_received->get_id() == 0);

  // Free the memory
  delete[] buffer;
  delete received_msg;

  exit(0);
}

void reply_case() {
  Reply reply0;
  reply0.set_target_id(1);
  reply0.set_sender_id(2);
  reply0.set_id(3);

  // Serialize the message
  Serializer serializer;
  reply0.serialize(serializer);
  unsigned char *buffer = serializer.get_serialized_buffer();
  size_t buffer_size = serializer.get_size_serialized_data();

  // Now deserialize it and make sure that all of the values from the
  // original has been retained
  Message *received_msg = Message::deserialize_as_message(buffer, buffer_size);

  // test
  Reply *reply_received = received_msg->as_reply();
  ASSERT_T(reply_received != nullptr);
  ASSERT_T(reply_received->get_target_id() == 1);
  ASSERT_F(reply_received->get_target_id() == 0);
  ASSERT_T(reply_received->get_sender_id() == 2);
  ASSERT_F(reply_received->get_sender_id() == 0);
  ASSERT_T(reply_received->get_id() == 3);
  ASSERT_F(reply_received->get_id() == 0);

  // Free the memory
  delete[] buffer;
  delete received_msg;

  exit(0);
}

void get_case() {
  Get get0;
  get0.set_target_id(1);
  get0.set_sender_id(2);
  get0.set_id(3);

  // Serialize the message
  Serializer serializer;
  get0.serialize(serializer);
  unsigned char *buffer = serializer.get_serialized_buffer();
  size_t buffer_size = serializer.get_size_serialized_data();

  // Now deserialize it and make sure that all of the values from the
  // original has been retained
  Message *received_msg = Message::deserialize_as_message(buffer, buffer_size);

  // test
  Get *get_received = received_msg->as_get();
  ASSERT_T(get_received != nullptr);
  ASSERT_T(get_received->get_target_id() == 1);
  ASSERT_F(get_received->get_target_id() == 0);
  ASSERT_T(get_received->get_sender_id() == 2);
  ASSERT_F(get_received->get_sender_id() == 0);
  ASSERT_T(get_received->get_id() == 3);
  ASSERT_F(get_received->get_id() == 0);

  // Free the memory
  delete[] buffer;
  delete received_msg;

  exit(0);
}

void waitAget_case() {
  WaitAndGet wait_and_get0;
  wait_and_get0.set_target_id(1);
  wait_and_get0.set_sender_id(2);
  wait_and_get0.set_id(3);

  // Serialize the message
  Serializer serializer;
  wait_and_get0.serialize(serializer);
  unsigned char *buffer = serializer.get_serialized_buffer();
  size_t buffer_size = serializer.get_size_serialized_data();

  // Now deserialize it and make sure that all of the values from the
  // original has been retained
  Message *received_msg = Message::deserialize_as_message(buffer, buffer_size);

  // test
  WaitAndGet *wag_received = received_msg->as_waitandget();
  ASSERT_T(wag_received != nullptr);
  ASSERT_T(wag_received->get_target_id() == 1);
  ASSERT_F(wag_received->get_target_id() == 0);
  ASSERT_T(wag_received->get_sender_id() == 2);
  ASSERT_F(wag_received->get_sender_id() == 0);
  ASSERT_T(wag_received->get_id() == 3);
  ASSERT_F(wag_received->get_id() == 0);

  // Free the memory
  delete[] buffer;
  delete received_msg;

  exit(0);
}

void kill_case() {
  Kill kill0;
  kill0.set_target_id(1);
  kill0.set_sender_id(2);
  kill0.set_id(3);

  // Serialize the message
  Serializer serializer;
  kill0.serialize(serializer);
  unsigned char *buffer = serializer.get_serialized_buffer();
  size_t buffer_size = serializer.get_size_serialized_data();

  // Now deserialize it and make sure that all of the values from the
  // original has been retained
  Message *received_msg = Message::deserialize_as_message(buffer, buffer_size);

  // test
  Kill *kill_received = received_msg->as_kill();
  ASSERT_T(kill_received != nullptr);
  ASSERT_T(kill_received->get_target_id() == 1);
  ASSERT_F(kill_received->get_target_id() == 0);
  ASSERT_T(kill_received->get_sender_id() == 2);
  ASSERT_F(kill_received->get_sender_id() == 0);
  ASSERT_T(kill_received->get_id() == 3);
  ASSERT_F(kill_received->get_id() == 0);

  // Free the memory
  delete[] buffer;
  delete received_msg;

  exit(0);
}

void register_case() {
  String *ip_addr = new String("127.0.0.1");
  Register register0(ip_addr, 4);
  register0.set_target_id(1);
  register0.set_sender_id(2);
  register0.set_id(3);

  // Serialize the message
  Serializer serializer;
  register0.serialize(serializer);
  unsigned char *buffer = serializer.get_serialized_buffer();
  size_t buffer_size = serializer.get_size_serialized_data();

  // Now deserialize it and make sure that all of the values from the
  // original has been retained
  Message *received_msg = Message::deserialize_as_message(buffer, buffer_size);

  // test
  Register *register_received_msg = received_msg->as_register();
  ASSERT_T(register_received_msg != nullptr);
  ASSERT_T(register_received_msg->get_target_id() == 1);
  ASSERT_F(register_received_msg->get_target_id() == 0);
  ASSERT_T(register_received_msg->get_sender_id() == 2);
  ASSERT_F(register_received_msg->get_sender_id() == 0);
  ASSERT_T(register_received_msg->get_id() == 3);
  ASSERT_F(register_received_msg->get_id() == 0);
  String *received_ip = register_received_msg->get_ip_addr();
  ASSERT_T(received_ip->equals(ip_addr));
  ASSERT_EQ(register_received_msg->get_port_num(), 4);

  // Free the memory
  delete[] buffer;
  delete received_msg;
  delete ip_addr;
  delete received_ip;

  exit(0);
}

void directory_case() {
  String *ip_addr = new String("127.0.0.1");
  Directory directory0(5);
  directory0.set_target_id(1);
  directory0.set_sender_id(2);
  directory0.set_id(3);

  directory0.add_client(0, ip_addr, 1);
  directory0.add_client(2, ip_addr, 4);
  directory0.add_client(3, ip_addr, 5);

  // Serialize the message
  Serializer serializer;
  directory0.serialize(serializer);
  unsigned char *buffer = serializer.get_serialized_buffer();
  size_t buffer_size = serializer.get_size_serialized_data();

  // Now deserialize it and make sure that all of the values from the
  // original has been retained
  Message *received_msg = Message::deserialize_as_message(buffer, buffer_size);

  // Test
  Directory *dir_received = received_msg->as_directory();
  ASSERT_T(dir_received != nullptr);
  ASSERT_T(dir_received->get_target_id() == 1);
  ASSERT_F(dir_received->get_target_id() == 0);
  ASSERT_T(dir_received->get_sender_id() == 2);
  ASSERT_F(dir_received->get_sender_id() == 0);
  ASSERT_T(dir_received->get_id() == 3);
  ASSERT_F(dir_received->get_id() == 0);

  ASSERT_EQ(dir_received->get_max_num_clients(), 5);
  ASSERT_T(dir_received->is_client_connected(0));
  ASSERT_F(dir_received->is_client_connected(1));
  ASSERT_T(dir_received->is_client_connected(2));
  ASSERT_T(dir_received->is_client_connected(3));
  ASSERT_F(dir_received->is_client_connected(4));
  ASSERT_EQ(dir_received->get_client_port_num(0), 1);
  ASSERT_EQ(dir_received->get_client_port_num(2), 4);
  ASSERT_EQ(dir_received->get_client_port_num(3), 5);
  String *recv_ip_addr_0 = dir_received->get_client_ip_addr(0);
  String *recv_ip_addr_2 = dir_received->get_client_ip_addr(2);
  String *recv_ip_addr_3 = dir_received->get_client_ip_addr(3);
  ASSERT_T(recv_ip_addr_0->equals(ip_addr));
  ASSERT_T(recv_ip_addr_2->equals(ip_addr));
  ASSERT_T(recv_ip_addr_3->equals(ip_addr));

  // free the memory
  delete[] buffer;
  delete received_msg;
  delete ip_addr;
  delete recv_ip_addr_0;
  delete recv_ip_addr_2;
  delete recv_ip_addr_3;

  exit(0);
}

TEST(serializer, test_int) { ASSERT_EXIT_ZERO(test_int); }
TEST(serializer, test_string) { ASSERT_EXIT_ZERO(test_string); }
TEST(serializer, test_size_t) { ASSERT_EXIT_ZERO(test_size_t); }
TEST(serializer, test_bool) { ASSERT_EXIT_ZERO(test_bool); }
TEST(messages, status_round_trip) { ASSERT_EXIT_ZERO(status_round_trip); }
TEST(messages, status_funcs) { ASSERT_EXIT_ZERO(status_funcs); }
TEST(messages, ack_case) { ASSERT_EXIT_ZERO(ack_case); }
TEST(messages, nack_case) { ASSERT_EXIT_ZERO(nack_case); }
TEST(messages, puck_case) { ASSERT_EXIT_ZERO(put_case); }
TEST(messages, reply_case) { ASSERT_EXIT_ZERO(reply_case); }
TEST(messages, get_case) { ASSERT_EXIT_ZERO(get_case); }
TEST(messages, waitAget_case) { ASSERT_EXIT_ZERO(waitAget_case); }
TEST(messages, kill_case) { ASSERT_EXIT_ZERO(kill_case); }
TEST(messages, register_case) { ASSERT_EXIT_ZERO(register_case); }
TEST(messages, directory_case) { ASSERT_EXIT_ZERO(directory_case); }

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
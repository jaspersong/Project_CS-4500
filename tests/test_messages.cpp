/**
 * Name: Snowy Chen, Joe Song
 * Date: 22 March 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// lang::Cpp

#include "helper.h"
#include "serial.h"

Sys helper;

void test_size_t() {
  Serializer serializer;

  // Serialize a bunch of size_t values
  serializer.set_size_t(1);
  serializer.set_size_t(2);
  serializer.set_size_t(3);

  unsigned char *buffer = serializer.get_serialized_buffer();
  size_t buffer_size = serializer.get_size_serialized_data();
  helper.t_true(buffer_size != 0);
  helper.t_true(buffer != nullptr);

  // Now deserialize
  Deserializer deserializer(buffer, buffer_size);
  helper.t_true(deserializer.get_num_bytes_left() > 0);
  helper.t_true(deserializer.has_size_t());
  helper.t_true(deserializer.get_size_t() == 1);

  helper.t_true(deserializer.get_num_bytes_left() > 0);
  helper.t_true(deserializer.has_size_t());
  helper.t_true(deserializer.get_size_t() == 2);

  helper.t_true(deserializer.get_num_bytes_left() > 0);
  helper.t_true(deserializer.has_size_t());
  helper.t_true(deserializer.get_size_t() == 3);

  helper.t_true(deserializer.get_num_bytes_left() == 0);
  helper.t_false(deserializer.has_size_t());

  delete[] buffer;

  helper.OK("Test test_size_t passed");
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
  helper.t_true(buffer_size != 0);
  helper.t_true(buffer != nullptr);

  // Now deserialize
  Deserializer deserializer(buffer, buffer_size);
  helper.t_true(deserializer.get_num_bytes_left() > 0);
  helper.t_true(deserializer.has_size_t());
  String *ret_hello = String::deserialize_as_string(deserializer);
  helper.t_true(ret_hello != nullptr);
  helper.t_true(ret_hello->equals(&hello));

  helper.t_true(deserializer.get_num_bytes_left() > 0);
  helper.t_true(deserializer.has_size_t());
  String *ret_world = String::deserialize_as_string(deserializer);
  helper.t_true(ret_world != nullptr);
  helper.t_true(ret_world->equals(&world));

  helper.t_true(deserializer.get_num_bytes_left() == 0);
  helper.t_false(deserializer.has_size_t());

  delete[] buffer;
  delete ret_hello;
  delete ret_world;

  helper.OK("Test test_string passed");
}

void test_int() {
  Serializer serializer;

  // Serialize a bunch of int values
  serializer.set_int(4);
  serializer.set_int(5);
  serializer.set_int(6);

  unsigned char *buffer = serializer.get_serialized_buffer();
  size_t buffer_size = serializer.get_size_serialized_data();
  helper.t_true(buffer_size != 0);
  helper.t_true(buffer != nullptr);

  // Now deserialize
  Deserializer deserializer(buffer, buffer_size);
  helper.t_true(deserializer.get_num_bytes_left() > 0);
  helper.t_true(deserializer.has_int());
  helper.t_true(deserializer.get_int() == 4);

  helper.t_true(deserializer.get_num_bytes_left() > 0);
  helper.t_true(deserializer.has_int());
  helper.t_true(deserializer.get_int() == 5);

  helper.t_true(deserializer.get_num_bytes_left() > 0);
  helper.t_true(deserializer.has_int());
  helper.t_true(deserializer.get_int() == 6);

  helper.t_true(deserializer.get_num_bytes_left() == 0);
  helper.t_false(deserializer.has_int());

  delete[] buffer;

  helper.OK("Test test_int passed");
}

void test_double() {
  Serializer serializer;

  // Serialize a bunch of int values
  serializer.set_double(4.2);
  serializer.set_int(5);
  serializer.set_double(6.3);

  unsigned char *buffer = serializer.get_serialized_buffer();
  size_t buffer_size = serializer.get_size_serialized_data();
  helper.t_true(buffer_size != 0);
  helper.t_true(buffer != nullptr);

  // Now deserialize
  Deserializer deserializer(buffer, buffer_size);
  helper.t_true(deserializer.get_num_bytes_left() > 0);
  helper.t_true(deserializer.has_double());
  helper.t_true(deserializer.get_double() == 4.2);

  helper.t_true(deserializer.get_num_bytes_left() > 0);
  helper.t_true(deserializer.has_int());
  helper.t_true(deserializer.get_int() == 5);

  helper.t_true(deserializer.get_num_bytes_left() > 0);
  helper.t_true(deserializer.has_double());
  helper.t_true(deserializer.get_double() == 6.3);

  helper.t_true(deserializer.get_num_bytes_left() == 0);
  helper.t_false(deserializer.has_double());

  delete[] buffer;

  helper.OK("Test test_double passed");
}

void test_bool() {
  Serializer serializer;

  // Serialize a bunch of size_t values
  serializer.set_bool(true);
  serializer.set_int(5);
  serializer.set_bool(false);

  unsigned char *buffer = serializer.get_serialized_buffer();
  size_t buffer_size = serializer.get_size_serialized_data();
  helper.t_true(buffer_size != 0);
  helper.t_true(buffer != nullptr);

  // Now deserialize
  Deserializer deserializer(buffer, buffer_size);
  helper.t_true(deserializer.get_num_bytes_left() > 0);
  helper.t_true(deserializer.has_bool());
  helper.t_true(deserializer.get_bool());

  helper.t_true(deserializer.get_num_bytes_left() > 0);
  helper.t_true(deserializer.has_int());
  helper.t_true(deserializer.get_int() == 5);

  helper.t_true(deserializer.get_num_bytes_left() > 0);
  helper.t_true(deserializer.has_bool());
  helper.t_false(deserializer.get_bool());

  helper.t_true(deserializer.get_num_bytes_left() == 0);
  helper.t_false(deserializer.has_bool());

  delete[] buffer;

  helper.OK("Test test_bool passed");
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
  helper.t_true(status_received_msg != nullptr);
  helper.t_true(status_received_msg->get_target_id() == 1);
  helper.t_false(status_received_msg->get_target_id() == 0);
  helper.t_true(status_received_msg->get_sender_id() == 2);
  helper.t_false(status_received_msg->get_sender_id() == 0);
  helper.t_true(status_received_msg->get_id() == 3);
  helper.t_false(status_received_msg->get_id() == 0);
  String *received_status = status_received_msg->get_message();
  helper.t_true(received_status->equals(&status_msg));

  // Free the memory as appropriate
  delete[] buffer;
  delete received_msg;
  delete received_status;

  helper.OK("Test status_round_trip passed");
}

void status_funcs() {
  String status_msg0("The fire nation attacked.\n");
  String status_msg1("Help I've fallen but don't want to get up.\n");
  Status status0(status_msg0);

  String *recv_msg0 = status0.get_message();
  helper.t_true(recv_msg0->equals(&status_msg0));

  status0.set_message(status_msg1);
  String *recv_msg1 = status0.get_message();
  helper.t_true(recv_msg1->equals(&status_msg1));

  // Free memory as appropriate
  delete recv_msg0;
  delete recv_msg1;

  helper.OK("Test status_funcs passed");
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
  helper.t_true(put_received != nullptr);
  helper.t_true(put_received->get_target_id() == 1);
  helper.t_false(put_received->get_target_id() == 0);
  helper.t_true(put_received->get_sender_id() == 2);
  helper.t_false(put_received->get_sender_id() == 0);
  helper.t_true(put_received->get_id() == 3);
  helper.t_false(put_received->get_id() == 0);

  // Free the memory
  delete[] buffer;
  delete received_msg;

  helper.OK("Test put_case passed");
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
  helper.t_true(reply_received != nullptr);
  helper.t_true(reply_received->get_target_id() == 1);
  helper.t_false(reply_received->get_target_id() == 0);
  helper.t_true(reply_received->get_sender_id() == 2);
  helper.t_false(reply_received->get_sender_id() == 0);
  helper.t_true(reply_received->get_id() == 3);
  helper.t_false(reply_received->get_id() == 0);

  // Free the memory
  delete[] buffer;
  delete received_msg;

  helper.OK("Test reply_case passed");
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
  helper.t_true(wag_received != nullptr);
  helper.t_true(wag_received->get_target_id() == 1);
  helper.t_false(wag_received->get_target_id() == 0);
  helper.t_true(wag_received->get_sender_id() == 2);
  helper.t_false(wag_received->get_sender_id() == 0);
  helper.t_true(wag_received->get_id() == 3);
  helper.t_false(wag_received->get_id() == 0);

  // Free the memory
  delete[] buffer;
  delete received_msg;

  helper.OK("Test waitAget_case passed");
}

void register_case() {
  auto *ip_addr = new String("127.0.0.1");
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
  helper.t_true(register_received_msg != nullptr);
  helper.t_true(register_received_msg->get_target_id() == 1);
  helper.t_false(register_received_msg->get_target_id() == 0);
  helper.t_true(register_received_msg->get_sender_id() == 2);
  helper.t_false(register_received_msg->get_sender_id() == 0);
  helper.t_true(register_received_msg->get_id() == 3);
  helper.t_false(register_received_msg->get_id() == 0);
  String *received_ip = register_received_msg->get_ip_addr();
  helper.t_true(received_ip->equals(ip_addr));
  helper.t_true(register_received_msg->get_port_num() == 4);

  // Free the memory
  delete[] buffer;
  delete received_msg;
  delete ip_addr;
  delete received_ip;

  helper.OK("Test register_case passed");
}

void directory_case() {
  auto *ip_addr = new String("127.0.0.1");
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
  helper.t_true(dir_received != nullptr);
  helper.t_true(dir_received->get_target_id() == 1);
  helper.t_false(dir_received->get_target_id() == 0);
  helper.t_true(dir_received->get_sender_id() == 2);
  helper.t_false(dir_received->get_sender_id() == 0);
  helper.t_true(dir_received->get_id() == 3);
  helper.t_false(dir_received->get_id() == 0);

  helper.t_true(dir_received->get_max_num_clients() == 5);
  helper.t_true(dir_received->is_client_connected(0));
  helper.t_false(dir_received->is_client_connected(1));
  helper.t_true(dir_received->is_client_connected(2));
  helper.t_true(dir_received->is_client_connected(3));
  helper.t_false(dir_received->is_client_connected(4));
  helper.t_true(dir_received->get_client_port_num(0) == 1);
  helper.t_true(dir_received->get_client_port_num(2) == 4);
  helper.t_true(dir_received->get_client_port_num(3) == 5);
  String *recv_ip_addr_0 = dir_received->get_client_ip_addr(0);
  String *recv_ip_addr_2 = dir_received->get_client_ip_addr(2);
  String *recv_ip_addr_3 = dir_received->get_client_ip_addr(3);
  helper.t_true(recv_ip_addr_0->equals(ip_addr));
  helper.t_true(recv_ip_addr_2->equals(ip_addr));
  helper.t_true(recv_ip_addr_3->equals(ip_addr));

  // free the memory
  delete[] buffer;
  delete received_msg;
  delete ip_addr;
  delete recv_ip_addr_0;
  delete recv_ip_addr_2;
  delete recv_ip_addr_3;

  helper.OK("Test directory_case passed");
}

int main(int argc, char **argv) {
  test_int();
  test_string();
  test_size_t();
  test_bool();
  test_double();
  status_round_trip();
  status_funcs();
  put_case();
  reply_case();
  waitAget_case();
  register_case();
  directory_case();
}
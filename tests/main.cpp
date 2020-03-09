/**
 * Name: Snowy Chen, Joe Song
 * Date: 28 February 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// lang::Cpp

#include "../src/serial.h"
#include "../../helper.h"

Sys helper;

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
  helper.t_true(status_received_msg != nullptr);
  helper.t_true(status_received_msg->get_target_id() == 1);
  helper.t_false(status_received_msg->get_target_id() == 0);
  helper.t_true(status_received_msg->get_sender_id() == 2);
  helper.t_false(status_received_msg->get_sender_id() == 0);
  helper.t_true(status_received_msg->get_id() == 3);
  helper.t_false(status_received_msg->get_id() == 0);
  String *received_status = status_received_msg->get_message();
  helper.t_true(received_status->equals(status_msg));

  // Free the memory as appropriate
  delete status_msg;
  delete status0;
  delete[] serialized_msg;
  delete received_msg;
  delete received_status;

  helper.OK("Passed serialization and deserialization of Status Message.\n");
}

void status_funcs() {
  String *status_msg0 = new String("The fire nation attacked.\n");
  String *status_msg1 = new String(
      "Help I've fallen but don't want to get up.\n");
  Status *status0 = new Status(*status_msg0);

  String *recv_msg0 = status0->get_message();
  helper.t_true(recv_msg0->equals(status_msg0));

  status0->set_message(*status_msg1);
  String *recv_msg1 = status0->get_message();
  helper.t_true(recv_msg1->equals(status_msg1));

  // Free memory as appropriate
  delete status_msg0;
  delete status_msg1;
  delete status0;
  delete recv_msg0;
  delete recv_msg1;

  helper.OK("Passed generalized function tests for Status message.\n");
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
  helper.t_true(ack_received_message != nullptr);
  helper.t_true(ack_received_message->get_target_id() == 1);
  helper.t_false(ack_received_message->get_target_id() == 0);
  helper.t_true(ack_received_message->get_sender_id() == 2);
  helper.t_false(ack_received_message->get_sender_id() == 0);
  helper.t_true(ack_received_message->get_id() == 3);
  helper.t_false(ack_received_message->get_id() == 0);


  // Free the memory
  delete ack0;
  delete[] serialized_msg;
  delete received_msg;

  helper.OK("Ack case has been pass.\n");

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
  helper.t_true(nack_received_message != nullptr);
  helper.t_true(nack_received_message->get_target_id() == 1);
  helper.t_false(nack_received_message->get_target_id() == 0);
  helper.t_true(nack_received_message->get_sender_id() == 2);
  helper.t_false(nack_received_message->get_sender_id() == 0);
  helper.t_true(nack_received_message->get_id() == 3);
  helper.t_false(nack_received_message->get_id() == 0);

  // Free the memory
  delete nack0;
  delete[] serialized_msg;
  delete received_msg;

  helper.OK("Nack case has been pass.\n");
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
  helper.t_true(put_received_message != nullptr);
  helper.t_true(put_received_message->get_target_id() == 1);
  helper.t_false(put_received_message->get_target_id() == 0);
  helper.t_true(put_received_message->get_sender_id() == 2);
  helper.t_false(put_received_message->get_sender_id() == 0);
  helper.t_true(put_received_message->get_id() == 3);
  helper.t_false(put_received_message->get_id() == 0);

  // Free the memory
  delete put0;
  delete[] serialized_msg;
  delete received_msg;

  helper.OK("Put case has been pass\n");
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
  helper.t_true(reply_received_message != nullptr);
  helper.t_true(reply_received_message->get_target_id() == 1);
  helper.t_false(reply_received_message->get_target_id() == 0);
  helper.t_true(reply_received_message->get_sender_id() == 2);
  helper.t_false(reply_received_message->get_sender_id() == 0);
  helper.t_true(reply_received_message->get_id() == 3);
  helper.t_false(reply_received_message->get_id() == 0);

  // Free the memory
  delete reply0;
  delete received_msg;
  delete[] serialized_msg;

  helper.OK("reply case has been pass\n");
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
  helper.t_true(get_received_msg != nullptr);
  helper.t_true(get_received_msg->get_target_id() == 1);
  helper.t_false(get_received_msg->get_target_id() == 0);
  helper.t_true(get_received_msg->get_sender_id() == 2);
  helper.t_false(get_received_msg->get_sender_id() == 0);
  helper.t_true(get_received_msg->get_id() == 3);
  helper.t_false(get_received_msg->get_id() == 0);

  // Free the memory
  delete get0;
  delete received_msg;
  delete[] serialized_msg;

  helper.OK("get case has been pass\n");
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
  helper.t_true(waitAget_received_msg != nullptr);
  helper.t_true(waitAget_received_msg->get_target_id() == 1);
  helper.t_false(waitAget_received_msg->get_target_id() == 0);
  helper.t_true(waitAget_received_msg->get_sender_id() == 2);
  helper.t_false(waitAget_received_msg->get_sender_id() == 0);
  helper.t_true(waitAget_received_msg->get_id() == 3);
  helper.t_false(waitAget_received_msg->get_id() == 0);

  // Free the memory
  delete waitAget0;
  delete received_msg;
  delete[] serialized_msg;

  helper.OK("wait and get case has been pass\n");

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
  helper.t_true(kill_received_msg != nullptr);
  helper.t_true(kill_received_msg->get_target_id() == 1);
  helper.t_false(kill_received_msg->get_target_id() == 0);
  helper.t_true(kill_received_msg->get_sender_id() == 2);
  helper.t_false(kill_received_msg->get_sender_id() == 0);
  helper.t_true(kill_received_msg->get_id() == 3);
  helper.t_false(kill_received_msg->get_id() == 0);

  // Free the memory
  delete kill0;
  delete received_msg;
  delete[] serialized_msg;

  helper.OK("kill case has been pass.\n");
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
  helper.t_true(register_received_msg != nullptr);
  helper.t_true(register_received_msg->get_target_id() == 1);
  helper.t_false(register_received_msg->get_target_id() == 0);
  helper.t_true(register_received_msg->get_sender_id() == 2);
  helper.t_false(register_received_msg->get_sender_id() == 0);
  helper.t_true(register_received_msg->get_id() == 3);
  helper.t_false(register_received_msg->get_id() == 0);

  // Free the memory
  delete register0;
  delete received_msg;
  delete[] serialized_msg;

  helper.OK("register case has been pass\n");
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
  helper.t_true(directory_received_message != nullptr);
  helper.t_true(directory_received_message->get_target_id() == 1);
  helper.t_false(directory_received_message->get_target_id() == 0);
  helper.t_true(directory_received_message->get_sender_id() == 2);
  helper.t_false(directory_received_message->get_sender_id() == 0);
  helper.t_true(directory_received_message->get_id() == 3);
  helper.t_false(directory_received_message->get_id() == 0);

  // free the memory
  delete directory0;
  delete received_msg;
  delete[] serialized_msg;

  helper.OK("directory case has been pass\n");
}



int main() {
  status_round_trip();
  status_funcs();
  ack_case();
  nack_case();
  put_case();
  reply_case();
  get_case();
  waitAget_case();
  kill_case();
  register_case();
  directory_case();

  return 0;
}
/**
 * Name: Snowy Chen, Joe Song
 * Date: 21 February 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// lang::Cpp

#include "network.h"
#include "helper.h"

Sys helper;

void test_server() {
  // Simply create the abstract server class in order to test the basic
  // functionality of the server that do not involve the
  // interaction between the client and server
  Server * dummy_server0 = new Server(new String("127.0.0.1"), 1234);
  Server * dummy_server1 = new Server(new String("127.0.0.1"), 1234, 30, 200);

  helper.t_false(dummy_server0->is_running());
  helper.t_true(dummy_server0->get_max_clients() == 1);
  helper.t_true(dummy_server0->get_max_receive_size() == 1024);
  helper.t_true(dummy_server1->get_max_clients() == 30);
  helper.t_true(dummy_server1->get_max_receive_size() == 200);

  delete dummy_server0;
  delete dummy_server1;

  helper.OK("Test test_server passed");
}

void test_client() {
  // Simply create the abstract client class in order to test the basic
  // functionality of the client that do not involve the interaction between
  // client and server
  Client * dummy_client0 = new Client(new String("127.0.0.1"), 1234);
  Client * dummy_client1 = new Client(new String("127.0.0.1"), 1234, 100);

  helper.t_false(dummy_client0->is_running());
  helper.t_true(dummy_client0->get_max_receive_size() == 1024);
  helper.t_true(dummy_client1->get_max_receive_size() == 100);

  delete dummy_client0;
  delete dummy_client1;

  helper.OK("Test test_client passed");
}

int main(int argc, char **argv) {
  test_server();
  test_client();
}
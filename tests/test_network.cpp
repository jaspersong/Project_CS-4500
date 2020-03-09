/**
 * Name: Snowy Chen, Joe Song
 * Date: 21 February 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// lang::Cpp

#include <gtest/gtest.h>

#include "network.h"

#define ASSERT_T(a) ASSERT_EQ((a), true)
#define ASSERT_F(a) ASSERT_EQ((a), false)
#define ASSERT_EXIT_ZERO(a) ASSERT_EXIT(a(), ::testing::ExitedWithCode(0), ".*")

void test_server() {
  // Simply create the abstract server class in order to test the basic
  // functionality of the server that do not involve the
  // interaction between the client and server
  Server * dummy_server0 = new Server(new String("127.0.0.1"), 1234);
  Server * dummy_server1 = new Server(new String("127.0.0.1"), 1234, 30, 200);

  ASSERT_F(dummy_server0->is_running());
  ASSERT_EQ(dummy_server0->get_max_clients(), 1);
  ASSERT_EQ(dummy_server0->get_max_receive_size(), 1024);
  ASSERT_EQ(dummy_server1->get_max_clients(), 30);
  ASSERT_EQ(dummy_server1->get_max_receive_size(), 200);

  delete dummy_server0;
  delete dummy_server1;

  exit(0);
}

void test_client() {
  // Simply create the abstract client class in order to test the basic
  // functionality of the client that do not involve the interaction between
  // client and server
  Client * dummy_client0 = new Client(new String("127.0.0.1"), 1234);
  Client * dummy_client1 = new Client(new String("127.0.0.1"), 1234, 100);

  ASSERT_F(dummy_client0->is_running());
  ASSERT_EQ(dummy_client0->get_max_receive_size(), 1024);
  ASSERT_EQ(dummy_client1->get_max_receive_size(), 100);

  delete dummy_client0;
  delete dummy_client1;

  exit(0);
}

TEST(A5, test_server) { ASSERT_EXIT_ZERO(test_server); }
TEST(A5, test_client) { ASSERT_EXIT_ZERO(test_client); }

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
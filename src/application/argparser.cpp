/**
 * Name: Snowy Chen, Joe Song
 * Date: 29 March 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::Cpp

#include "argparser.h"
#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <cstring>

ArgParser::ArgParser(ArgParser::ParseTypes parse_type, int argc, char **argv) {
  this->type = parse_type;
  this->listener_ip_addr = nullptr;
  this->listener_port_num = 0;
  this->registrar_ip_addr = nullptr;
  this->registrar_port_num = 0;
  this->wordcount_file = nullptr;
  this->expected_num_nodes = 3; // Make 3 be the default number

  this->parse(argc, argv);
}
const char *ArgParser::get_listener_addr() {
  assert(this->listener_ip_addr);
  assert(this->listener_port_num != 0);

  return this->listener_ip_addr;
}

int ArgParser::get_listener_port() {
  assert(this->listener_ip_addr);
  assert(this->listener_port_num != 0);

  return this->listener_port_num;
}

const char *ArgParser::get_registrar_addr() {
  assert((this->type != ParseTypes::DemoRegistrar) &&
         (this->type != ParseTypes::WordCountRegistrar) &&
         (this->type != ParseTypes::LinusRegistrar));
  assert(this->listener_ip_addr);
  assert(this->listener_port_num != 0);
  assert(this->registrar_ip_addr);
  assert(this->registrar_port_num != 0);

  return this->registrar_ip_addr;
}

int ArgParser::get_registrar_port() {
  assert((this->type != ParseTypes::DemoRegistrar) &&
         (this->type != ParseTypes::WordCountRegistrar) &&
         (this->type != ParseTypes::LinusRegistrar));
  assert(this->listener_ip_addr);
  assert(this->listener_port_num != 0);
  assert(this->registrar_ip_addr);
  assert(this->registrar_port_num != 0);

  return this->registrar_port_num;
}

const char *ArgParser::get_wordcount_file() {
  assert(this->type == ParseTypes::WordCountRegistrar);
  assert(this->listener_ip_addr);
  assert(this->listener_port_num != 0);
  assert(this->wordcount_file);

  return this->wordcount_file;
}

size_t ArgParser::get_num_expected_nodes() {
  assert((this->type != ParseTypes::DemoNode) &&
         (this->type != ParseTypes::DemoRegistrar));
  assert(this->listener_ip_addr);
  assert(this->listener_port_num != 0);
  assert(this->expected_num_nodes > 0);

  return this->expected_num_nodes;
}

void ArgParser::parse(int argc, char **argv) {
  // Iterate through each of the arguments to get the values we need
  for (size_t i = 1; i < argc; i++) {
    // See what argument flag we see
    if (strcasecmp("--help", argv[i]) == 0) {
      // Doesn't matter what we're doing. We're going to print out the help
      // menu and stop the application from there
      this->print_help();
      exit(0);
    } else if (strcasecmp("--registrar", argv[i]) == 0) {
      // Get the registrar IP address and the port number
      assert(i + 2 < argc);
      this->registrar_ip_addr = argv[i + 1];
      this->registrar_port_num = atoi(argv[i + 2]);

      i += 2;
    } else if (strcasecmp("--listener", argv[i]) == 0) {
      // Get the listener IP address and the port number
      assert(i + 2 < argc);
      this->listener_ip_addr = argv[i + 1];
      this->listener_port_num = atoi(argv[i + 2]);

      i += 2;
    } else if (strcasecmp("--nodes", argv[i]) == 0) {
      assert(i + 1 < argc);
      this->expected_num_nodes = atoi(argv[i + 1]);
      i += 1;
    } else if (strcasecmp("--file", argv[i]) == 0) {
      assert(i + 1 < argc);
      this->wordcount_file = argv[i + 1];
      i += 1;
    } else {
      printf("Invalid argument.\n");
      exit(1);
    }
  }
}

void ArgParser::print_help() {
  printf("    --help: Print out the help menu of the application\n");

  switch (this->type) {
  case ParseTypes::DemoRegistrar:
    this->print_listener_help();
    this->print_registrar_help();
    break;
  case ParseTypes::DemoNode:
    this->print_listener_help();
    break;
  case ParseTypes::WordCountRegistrar:
    this->print_listener_help();
    this->print_registrar_help();
    this->print_wordcount_file_help();
    this->print_nodes_help();
    break;
  case ParseTypes::WordCountNode:
    this->print_listener_help();
    this->print_nodes_help();
    break;
  case ParseTypes::LinusRegistrar:
    this->print_listener_help();
    this->print_registrar_help();
    this->print_nodes_help();
    break;
  case ParseTypes::LinusNode:
    this->print_listener_help();
    this->print_nodes_help();
    break;
  default:
    printf("Invalid application argument format.\n");
    exit(1);
  }
}

void ArgParser::print_listener_help() {
  printf("    --listener: Specify the IP address and the port number that the"
         " node or registrar will listen for incoming connections. Example: "
         "--listener 127.0.0.1 1235\n");
}

void ArgParser::print_registrar_help() {
  printf("    --registrar: Specify the IP address and the port number that "
         "the reigstrar listens from. Example: --registrar 127.0.0.1 1234\n");
}

void ArgParser::print_wordcount_file_help() {
  printf("    --file: Specify the file that the word counter will read\n");
}

void ArgParser::print_nodes_help() {
  printf("    --nodes: Specify the number of nodes that the application will "
         "be using. The number includes the registrar. Default: 3");
}

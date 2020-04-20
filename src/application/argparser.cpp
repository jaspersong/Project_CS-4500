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

ArgParser::ArgParser(ArgParser::ParseTypes parse_type, int argc, char **argv) {
  this->type = parse_type;
  this->listener_ip_addr = nullptr;
  this->listener_port_num = 0;
  this->registrar_ip_addr = nullptr;
  this->registrar_port_num = 0;
  this->wordcount_file = nullptr;

  switch (this->type) {
  case ParseTypes::Registrar:
    this->parse_as_registrar(argc, argv);
    break;
  case ParseTypes::Node:
    this->parse_as_node(argc, argv);
    break;
  case ParseTypes::WordCountRegistrar:
    this->parse_as_wc_registrar(argc, argv);
    break;
  case ParseTypes::WordCountNode:
    this->parse_as_wc_node(argc, argv);
    break;
  default:
    assert(false); // should not get here
    break;
  }
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
  assert((this->type != ParseTypes::Registrar) &&
         (this->type != ParseTypes::WordCountRegistrar));
  assert(this->listener_ip_addr);
  assert(this->listener_port_num != 0);
  assert(this->registrar_ip_addr);
  assert(this->registrar_port_num != 0);

  return this->registrar_ip_addr;
}

int ArgParser::get_registrar_port() {
  assert((this->type != ParseTypes::Registrar) &&
         (this->type != ParseTypes::WordCountRegistrar));
  assert(this->listener_ip_addr);
  assert(this->listener_port_num != 0);
  assert(this->registrar_ip_addr);
  assert(this->registrar_port_num != 0);

  return this->registrar_port_num;
}

const char *ArgParser::get_wordcount_file() {
  assert((this->type == ParseTypes::WordCountRegistrar) ||
         (this->type == ParseTypes::WordCountNode));
  assert(this->listener_ip_addr);
  assert(this->listener_port_num != 0);
  assert(this->wordcount_file);

  return this->wordcount_file;
}

void ArgParser::parse_as_registrar(int argc, char **argv) {
  assert(argc >= 3);

  this->listener_ip_addr = argv[1];
  this->listener_port_num = atoi(argv[2]);
}

void ArgParser::parse_as_node(int argc, char **argv) {
  assert(argc >= 5);

  this->registrar_ip_addr = argv[1];
  this->registrar_port_num = atoi(argv[2]);
  this->listener_ip_addr = argv[3];
  this->listener_port_num = atoi(argv[4]);
}

void ArgParser::parse_as_wc_registrar(int argc, char **argv) {
  assert(argc >= 4);

  this->listener_ip_addr = argv[1];
  this->listener_port_num = atoi(argv[2]);
  this->wordcount_file = argv[3];
}

void ArgParser::parse_as_wc_node(int argc, char **argv) {
  assert(argc >= 6);

  this->registrar_ip_addr = argv[1];
  this->registrar_port_num = atoi(argv[2]);
  this->listener_ip_addr = argv[3];
  this->listener_port_num = atoi(argv[4]);
  this->wordcount_file = argv[5];
}

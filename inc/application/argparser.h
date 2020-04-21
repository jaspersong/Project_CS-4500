/**
 * Name: Snowy Chen, Joe Song
 * Date: 19 April 2020
 * Section: Jason Hemann, MR 11:45-1:25
 * Email: chen.xinu@husky.neu.edu, song.jo@husky.neu.edu
 */

// Lang::Cpp

#pragma once

#include <cstdlib>

/**
 * Argument parser for the demo applications. It has a select few types of
 * ways arguments can be parsed.
 */
class ArgParser {
public:
  enum class ParseTypes {
    DemoRegistrar,
    DemoNode,
    LinusRegistrar,
    LinusNode,
    WordCountRegistrar,
    WordCountNode,
  };

  ArgParser(ParseTypes parse_type, int argc, char **argv);

  const char *get_listener_addr();
  int get_listener_port();
  const char *get_registrar_addr();
  int get_registrar_port();
  const char *get_wordcount_file();

  size_t get_num_expected_nodes();

private:
  ParseTypes type;

  const char *listener_ip_addr;
  int listener_port_num;

  const char *registrar_ip_addr;
  int registrar_port_num;

  const char *wordcount_file;
  size_t expected_num_nodes;

  void parse(int argc, char **argv);
  void print_help();
  void print_listener_help();
  void print_registrar_help();
  void print_wordcount_file_help();
  void print_nodes_help();
};

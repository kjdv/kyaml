#include "structure_clauses.hh"
#include "clause_test.hh"
#include <iostream>
#include <gtest/gtest.h>

using namespace std;
using namespace kyaml::test;
using namespace kyaml::clauses;

namespace kyaml
{
  namespace test
  {
    template<>
    void setup(separate_in_line &sil)
    {
      sil.advance();
    }
  }
}

namespace
{
  //// separate in line 
  clause_testcase sil_tc(string const &input, bool result, unsigned indent_level, string const &value)
  {
    return 
      testcase_builder(input, result).
      with_indent_level(indent_level).
      with_consumed(result ? (indent_level + 1) : 1).
      with_value(value).
      build();
  }

  //// line prefix
 
  clause_testcase lp_tc(string const &input, unsigned indent_level, context::blockflow_t bf, bool result)
  {
    size_t n = input.find_first_not_of(" \t");
    unsigned consumed = 
      result ?
      ((n == string::npos) ? input.size() : n) :
      0;

    string value = input.substr(0, consumed);

    return
      testcase_builder(input, result).
      with_blockflow(bf).
      with_indent_level(indent_level).
      with_consumed(consumed).
      with_value(value).
      build();
  }

  //// empty line

  clause_testcase el_tc(string const &input, unsigned indent_level, context::blockflow_t bf, bool result)
  {
    size_t n = input.find_first_not_of(" \t\r\n");
    unsigned consumed =
      result ?
      ((n == string::npos) ? input.size() : n) :
      0;
    string value = input.substr(0, consumed);
    return
      testcase_builder(input, result).
      with_indent_level(indent_level).
      with_blockflow(bf).
      with_consumed(consumed).
      with_value(value).
      build();
  }
  
  //// trimmed
  clause_testcase tr_tc(string const &input, unsigned il, bool result, unsigned consumed, string const &value)
  {
    return 
      testcase_builder(input, result).
      with_indent_level(il).
      with_consumed(consumed).
      with_value(value).
      build();
  }

  // flow_folded
  clause_testcase  ff_tc(string const &input, unsigned il, context::blockflow_t bf, bool result, unsigned consumed, string const &value)
  {
    return 
      testcase_builder(input, result).
      with_indent_level(il).
      with_blockflow(bf).
      with_consumed(consumed).
      with_value(value).
      build();
  }
}

CLAUSE_TEST(separate_in_line, 
            cases({sil_tc("s ", true, 1, " "),
                  sil_tc("s  ", true, 2, "  "),
                  sil_tc("s", false, 0, ""),
                  sil_tc("sa", false, 0, ""),
                  sil_tc("s \t ", true, 3, " \t "),
                  sil_tc("sa ", false, 1, " "),
                  sil_tc("\n ", true, 1, " "),
                  sil_tc("\na", true, 0, ""),
                  sil_tc("\rb", true, 0, "")}))

CLAUSE_TEST(line_prefix,
            cases({lp_tc(" ", 1, context::NA, false),
                   lp_tc(" ", 1, context::BLOCK_OUT, true),
                   lp_tc("  ", 2, context::BLOCK_IN, true),
                   lp_tc("  identifier", 2, context::BLOCK_IN, true),
                   lp_tc("   identifier", 2, context::BLOCK_IN, false),
                   lp_tc("   identifier", 2, context::FLOW_IN, true),
                   lp_tc("   identifier", 1, context::FLOW_OUT, true),
                   lp_tc("     identifier", 2, context::FLOW_OUT, true)}))
 

CLAUSE_TEST(empty_line,
            cases({el_tc(" ", 1, context::NA, false),
                   el_tc(" \n", 2, context::NA, true),
                   el_tc(" \r", 2, context::NA, true),
                   el_tc(" \r\n", 3, context::NA, true),
                   el_tc("a", 3, context::NA, false),
                   el_tc("  \n", 3, context::BLOCK_IN, true),
                   el_tc("  \n", 2, context::BLOCK_IN, true),
                   el_tc("  \n", 1, context::FLOW_IN, true)}))

CLAUSE_TEST(trimmed,
            cases({tr_tc("\n  \n aap", 3, true, 4, "\n  \n"),
                  tr_tc("\r\n   \n  \n a", 4, true, 9, "\r\n   \n  \n"),
                  tr_tc("\na", 2, false, 0, "\n")}))

CLAUSE_TEST(line_folded,
            cases({tr_tc("\n  \n aap", 3, true, 4, "\n  \n"),
                  tr_tc("\r\n   \n  \n a", 4, true, 9, "\r\n   \n  \n"),
                  tr_tc("\na", 2, true, 1, "\n")}))

CLAUSE_TEST(flow_folded,
            cases({ff_tc("\n\n  \n    aap", 3, context::FLOW_IN, true, 9, "\n\n  \n    "),
                  ff_tc("\n\n  \n    aap", 3, context::FLOW_OUT, false, 0, "")}))

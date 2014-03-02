#include "separation_line_clauses.hh"
#include "clause_test.hh"
#include <gtest/gtest.h>

using namespace std;
using namespace kyaml::test;
using namespace kyaml::clauses;

namespace
{
  clause_testcase sl_tc(string const &input, bool result, unsigned indent_level, unsigned consume)
  {
    string value = input.substr(0, consume);
    return 
      testcase_builder(input, result).
      with_indent_level(indent_level).
      with_consumed(consume).
      with_value(value).
      build();
  }
}

CLAUSE_TEST(separate_in_lines,
            cases({
                sl_tc("\n# comment\n  a", true, 2, 13),
                sl_tc("  # blah\n     65", true, 3, 14),
                  }))

#ifdef COMPILE_GUARD

#include "separation_line_clauses.hh"
#include "clause_test.hh"
#include <gtest/gtest.h>

using namespace std;
using namespace kyaml::test;
using namespace kyaml::clauses;

namespace
{
  clause_testcase<void_result> sl_tc(string const &input, bool result, unsigned indent_level, unsigned consume)
  {
    return 
      testcase_builder<void_result>(input, result).
      with_indent_level(indent_level).
      with_consumed(consume).
      build();
  }
}

CLAUSE_TEST(separate_in_lines,
            cases({
                sl_tc("\n# comment\n  a", true, 2, 13),
                sl_tc("  # blah\n     65", true, 3, 14),
                  }))

#endif // COMPILE_GUARD

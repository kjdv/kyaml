#include "comment_clauses.hh"
#include "clause_test.hh"
#include <gtest/gtest.h>

using namespace std;
using namespace kyaml::test;
using namespace kyaml::clauses;

namespace
{
  clause_testcase tc(string const &input, bool result, string const &val)
  {
    return 
      testcase_builder(input, result).
      with_consumed(result ? val.size() : 0).
      build();
  }

  clause_testcase sl_tc(string const &input, bool result, unsigned c)
  {
    return 
      testcase_builder(input, result).
      with_consumed(c).
      build();
  }
}

CLAUSE_TEST(non_break_comment_text,
            cases({tc("# klaas", true, "# klaas"),
                   tc("## klaas", true, "## klaas"),
                   tc("#", true, "#"),
                   tc("klaas", false, ""),
                   tc("# klaas\n", true, "# klaas")}))

CLAUSE_TEST(sbreak_comment,
            cases({tc("#klaas\n", true, "#klaas\n")}))

CLAUSE_TEST(sline_comment,
            cases({sl_tc("    # Comment\n        # lines\n", true, 30),
                  sl_tc("\n# comment\n", true, 11),
                  sl_tc("\n# comment\n  a", true, 11)}))

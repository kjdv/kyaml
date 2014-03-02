#ifdef COMPILE_GUARD

#include "char_clauses.hh"
#include "clause_test.hh"
#include <gtest/gtest.h>

using namespace std;
using namespace kyaml;
using namespace kyaml::test;
using namespace kyaml::clauses;
using namespace kyaml::clauses::internal;

// internal test for any_of and all_of

namespace 
{
  clause_testcase<string_result> a_tc(string const &input, bool result, string val, unsigned c = 1)
  {
    return
      testcase_builder<string_result>(input, result).
      with_value(val).
      with_consumed(result ? c : 0).
      build();
  }
}

typedef internal::any_of<string_result,
                         simple_char_clause<'a'>,
                         simple_char_clause<'b'>,
                         simple_char_clause<'c'> > any_of_abc;

typedef internal::all_of<string_result,
                         simple_char_clause<'a'>,
                         simple_char_clause<'b'>,
                         simple_char_clause<'c'> > all_of_abc;

NAMED_CLAUSE_TEST(any_of_test,
                  any_of_abc,
                  cases({a_tc("a", true, "a"),
                         a_tc("b", true, "b"),
                         a_tc("c", true, "c"),
                         a_tc("d", false, "d")}))
                  
NAMED_CLAUSE_TEST(all_of_test,
                  all_of_abc,
                  cases({a_tc("abc", true, "abc", 3),
                        a_tc("bca", false, "0")}))
                  
#endif // COMPILE_GUARD

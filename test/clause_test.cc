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
  clause_testcase a_tc(string const &input, bool result, unsigned c = 1)
  {
    return
      testcase_builder(input, result).
      with_consumed(result ? c : 0).
      build();
  }
}

typedef internal::any_of<simple_char_clause<'a'>,
                         simple_char_clause<'b'>,
                         simple_char_clause<'c'> > any_of_abc;

typedef internal::all_of<simple_char_clause<'a'>,
                         simple_char_clause<'b'>,
                         simple_char_clause<'c'> > all_of_abc;

NAMED_CLAUSE_TEST(any_of_test,
                  any_of_abc,
                  cases({a_tc("a", true),
                         a_tc("b", true),
                         a_tc("c", true),
                         a_tc("d", false)}))
                  
NAMED_CLAUSE_TEST(all_of_test,
                  all_of_abc,
                  cases({a_tc("abc", true, 3),
                        a_tc("bca", false)}))

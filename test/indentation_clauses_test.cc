#include "indentation_clauses.hh"
#include "clause_test.hh"
#include <iostream>
#include <gtest/gtest.h>

using namespace std;
using namespace kyaml::test;
using namespace kyaml::clauses;

namespace 
{
  clause_testcase tc(string const &input, unsigned indent_level, bool result, unsigned consume)
  {
    clause_testcase t = 
    {
      input,
      indent_level,
      context::NA,
      result,
      consume,
    };
    return t;
  }
}

CLAUSE_TEST(indent_clause_eq, 
            cases({
                tc(" ", 1, true, 1),
                tc(" ", 2, false, 0),
                tc("  ", 2, true, 2),
                tc("", 1, false, 0),
                tc("", 0, false, 0),
                tc("    ", 2, false, 0),
                tc("  nonwhite", 2, true, 2),
                tc(" nonwhite", 2, false, 0),
                tc("   nonwhite", 2, false, 0),
                  }))

CLAUSE_TEST(indent_clause_lt, 
            cases({
                  tc("  ", 3, true, 2),
                  tc("   ", 3, false, 0),
                  tc("    ", 3, false, 0),
                  tc("", 1, true, 0),
                  tc("  nonwhite", 2, false, 0),
                  tc(" nonwhite", 2, true, 1),
                  tc("   nonwhite", 2, false, 0),
                  }))

CLAUSE_TEST(indent_clause_le,
            cases({
                  tc("  ", 3, true, 2),
                  tc("   ", 3, true, 3),
                  tc("    ", 3, false, 0),
                  tc("", 1, true, 0),
                  tc("  nonwhite", 2, true, 2),
                  tc(" nonwhite", 2, true, 1),
                  tc("   nonwhite", 2, false, 0),
                  }))

#include "indentation_clauses.hh"
#include <string>
#include <sstream>
#include <iostream>
#include <gtest/gtest.h>

using namespace std;
using namespace kyaml;
using namespace kyaml::clauses;

struct indent_clause_test_case
{
  string input;
  unsigned n;
  bool result;
};

namespace std
{
  ostream &operator<<(ostream &o, indent_clause_test_case const &ic)
  {
    return o << "input = \"" << ic.input 
             << "\" n = " << ic.n 
             << " expected result = " << (ic.result ? "true" : "false");
  }
}

template <typename clause_t>
class indent_clause_test_base : public testing::TestWithParam<indent_clause_test_case>
{
public:
  indent_clause_test_base() :
    d_stream(GetParam().input),
    d_cstream(d_stream),
    d_clause(d_cstream, GetParam().n)
  {}

  clause_t &clause()
  {
    return d_clause;
  }
  
  char_stream &stream()
  {
    return d_cstream;
  }

private:
  stringstream d_stream;
  char_stream d_cstream;
  clause_t d_clause;
};

typedef indent_clause_test_base<indent_clause_eq> indent_clause_eq_test;

TEST_P(indent_clause_eq_test, match)
{
  EXPECT_EQ(GetParam().result, clause().try_clause()) << GetParam();
}

TEST_P(indent_clause_eq_test, pos)
{    
  clause().try_clause();

  char_stream::mark_t expected_pos = GetParam().result ? GetParam().n : 0;
  EXPECT_EQ(expected_pos, stream().pos());
}

indent_clause_test_case indent_clause_eq_test_cases[] = 
{
  {" ", 1, true},
  {" ", 2, false},
  {"  ", 2, true},
  {"", 1, false},
  {"", 0, false},
  {"    ", 2, false},
  {"  nonwhite", 2, true},
  {" nonwhite", 2, false},
  {"   nonwhite", 2, false},
};

INSTANTIATE_TEST_CASE_P(indent_clause_eq_tests,
                        indent_clause_eq_test,
                        testing::ValuesIn(indent_clause_eq_test_cases));

typedef indent_clause_test_base<indent_clause_lt> indent_clause_lt_test;

TEST_P(indent_clause_lt_test, match)
{
  EXPECT_EQ(GetParam().result, clause().try_clause()) << GetParam();
}

TEST_P(indent_clause_lt_test, pos)
{
  clause().try_clause();
  char_stream::mark_t max_pos = GetParam().result ? GetParam().n : 0;
  if(max_pos)
    EXPECT_LT(stream().pos(), max_pos);
  else
    EXPECT_EQ(0, stream().pos());
}

TEST_P(indent_clause_lt_test, value)
{
  clause().try_clause();
  EXPECT_EQ(stream().pos(), clause().value());
}

indent_clause_test_case indent_clause_lt_test_cases[] = 
{
  {"  ", 3, true},
  {"   ", 3, false},
  {"    ", 3, false},
  {"", 1, true},
  {"  nonwhite", 2, false},
  {" nonwhite", 2, true},
  {"   nonwhite", 2, false},
};

INSTANTIATE_TEST_CASE_P(indent_clause_lt_tests,
                        indent_clause_lt_test,
                        testing::ValuesIn(indent_clause_lt_test_cases));

typedef indent_clause_test_base<indent_clause_le> indent_clause_le_test;

TEST_P(indent_clause_le_test, match)
{
  EXPECT_EQ(GetParam().result, clause().try_clause()) << GetParam();
}

TEST_P(indent_clause_le_test, pos)
{
  clause().try_clause();
  char_stream::mark_t max_pos = GetParam().result ? GetParam().n : 0;
  if(max_pos)
    EXPECT_LE(stream().pos(), max_pos);
  else
    EXPECT_EQ(0, stream().pos());
}

TEST_P(indent_clause_le_test, value)
{
  clause().try_clause();
  EXPECT_EQ(stream().pos(), clause().value());
}

indent_clause_test_case indent_clause_le_test_cases[] = 
{
  {"  ", 3, true},
  {"   ", 3, true},
  {"    ", 3, false},
  {"", 1, true},
  {"  nonwhite", 2, true},
  {" nonwhite", 2, true},
  {"   nonwhite", 2, false},
};

INSTANTIATE_TEST_CASE_P(indent_clause_le_tests,
                        indent_clause_le_test,
                        testing::ValuesIn(indent_clause_le_test_cases));

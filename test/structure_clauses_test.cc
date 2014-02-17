#include "structure_clauses.hh"
#include <string>
#include <sstream>
#include <iostream>
#include <gtest/gtest.h>

using namespace std;
using namespace kyaml;
using namespace kyaml::clauses;

struct separate_in_line_test_case
{
  string input;
  unsigned n;
  bool result;
};

namespace std
{
  ostream &operator<<(ostream &o, separate_in_line_test_case const &sc)
  {
    return o << "input = \"" << sc.input 
             << "\" n = " << sc.n 
             << " expected result = " << (sc.result ? "true" : "false");
  }
}

class separate_in_line_test : public testing::TestWithParam<separate_in_line_test_case>
{
public:
  separate_in_line_test() :
    d_stream(GetParam().input),
    d_cstream(d_stream),
    d_clause(d_cstream)
  {
    d_clause.advance();
  }

  separate_in_line &clause()
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
  separate_in_line d_clause;
};

TEST_P(separate_in_line_test, match)
{
  EXPECT_EQ(GetParam().result, clause().try_clause()) << GetParam();
}

TEST_P(separate_in_line_test, pos)
{    
  clause().try_clause();

  char_stream::mark_t expected_pos = GetParam().result ? GetParam().n : 0;
  EXPECT_EQ(expected_pos + 1, stream().pos());
}

separate_in_line_test_case separate_in_line_test_cases[] = 
{
  {"s ", 1, true},
  {"s  ", 2, true},
  {"s", 0, false},
  {"sa", 0, false},
  {"s \t ", 3, true},
  {"sa ", 1, false},
  {"\n ", 1, true},
  {"\na", 0, true},
  {"\rb", 0, true},
};

INSTANTIATE_TEST_CASE_P(separate_in_line_tests,
                        separate_in_line_test,
                        testing::ValuesIn(separate_in_line_test_cases));

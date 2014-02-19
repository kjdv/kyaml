#ifndef CLAUSE_TEST_HH
#define CLAUSE_TEST_HH

#include "clauses_base.hh"
#include <iostream>
#include <vector>
#include <initializer_list>
#include <gtest/gtest.h>

struct clause_testcase
{
  std::string input;
  unsigned indent_level;
  kyaml::clauses::context::blockflow_t blockflow;

  bool const result;
  unsigned const consumed;

  std::string const value;
};

inline std::vector<clause_testcase> cases(std::initializer_list<clause_testcase> il)
{
  return std::vector<clause_testcase>(il);
}

namespace std
{
  inline ostream &operator<<(ostream &o, clause_testcase const &tc)
  {
    return o << "\n"
             << "input = \"" << tc.input << "\"\n"
             << "indent_level = " << tc.indent_level << "\n"
             << "blockflow = " << tc.blockflow << "\n"
             << "result = " << (tc.result ? "true" : "false") << "\n"
             << "consumed = " << tc.consumed << "\n"
             << "value = " << tc.value << "\n";
  }
}

template <typename clause_t>
void setup(clause_t &)
{}

template <typename clause_t>
void teardown(clause_t &)
{}

template <typename clause_t>
class clause_test : public testing::TestWithParam<clause_testcase>
{
public:
  clause_test() :
    d_ctx(GetParam().input, GetParam().indent_level, GetParam().blockflow),
    d_clause(d_ctx.get())
  {}

  void SetUp() override
  {
    setup(d_clause);
  }

  void TearDown() override
  {
    teardown(d_clause);
  }

  clause_t &clause()
  {
    return d_clause;
  }
  
  kyaml::clauses::context &ctx()
  {
    return d_ctx.get();
  }

  // tests:
  void test_match()
  {
    bool const expected = GetParam().result;
    EXPECT_EQ(expected, clause().try_clause());
  }

  void test_advance()
  {
    size_t const expected = GetParam().consumed;
    clause().try_clause();
    EXPECT_EQ(expected, ctx().stream().pos());
  }

  void test_value()
  {
    if(GetParam().result)
    {
      clause().try_clause();

      kyaml::clauses::string_result actual;
      actual.append(clause().value());
      EXPECT_EQ(GetParam().value, actual);
    }
  }

private:
  kyaml::clauses::context_wrap d_ctx;
  clause_t d_clause;
};

#define CLAUSE_TEST(clausetype, values)                         \
  typedef clause_test<clausetype> test_##clausetype;            \
                                                                \
  TEST_P(test_##clausetype, match)                              \
  {                                                             \
    test_match();                                               \
  }                                                             \
                                                                \
  TEST_P(test_##clausetype, advance)                            \
  {                                                             \
    test_advance();                                             \
  }                                                             \
  TEST_P(test_##clausetype, value)                              \
  {                                                             \
    test_value();                                               \
  }                                                             \
                                                                \
  INSTANTIATE_TEST_CASE_P(tests_##clausetype,                   \
                          test_##clausetype,                    \
                          testing::ValuesIn(values));


#endif // CLAUSE_TEST_HH

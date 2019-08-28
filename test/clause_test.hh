#ifndef CLAUSE_TEST_HH
#define CLAUSE_TEST_HH

#include "clauses_base.hh"
#include <iostream>
#include <vector>
#include <initializer_list>
#include <gtest/gtest.h>

namespace kyaml
{
  namespace test
  {
    class context_wrap
    {
    public:
      context_wrap(std::string const &s,
                   int indent_level = -1,
                   kyaml::context::blockflow_t bf = kyaml::context::NA) :
        d_sstream(s),
        d_stream(d_sstream),
        d_ctx(d_stream, indent_level, bf)
      {}

      kyaml::context const &get() const
      {
        return d_ctx;
      }
      
      kyaml::context &get()
      {
        return d_ctx;
      }
    private:
      std::stringstream d_sstream;
      char_stream d_stream;
      kyaml::context d_ctx;
    };

    struct clause_testcase
    {
      std::string input;
      int indent_level;
      kyaml::context::blockflow_t blockflow;

      bool const result;
      size_t const consumed;
    };

    class testcase_builder
    {
    public:
      testcase_builder(std::string const &input, bool result) :
        d_input(input),
        d_indent_level(0),
        d_blockflow(kyaml::context::NA),
        d_result(result),
        d_consumed(input.size())
      {}

      testcase_builder &with_indent_level(int l)
      {
        d_indent_level = l;
        return *this;
      }
      
      testcase_builder &with_blockflow(kyaml::context::blockflow_t bf)
      {
        d_blockflow = bf;
        return *this;
      }

      testcase_builder &with_consumed(unsigned n)
      {
        d_consumed = n;
        return *this;
      }

      clause_testcase build()
      {
        clause_testcase result = {d_input,
                                  d_indent_level,
                                  d_blockflow,
                                  d_result,
                                  d_consumed};
        return result;
      }
    private:
      std::string d_input;
      int d_indent_level;
      kyaml::context::blockflow_t d_blockflow;
      
      bool d_result;
      size_t d_consumed;
      
      std::string d_value;
    };

    inline std::vector<clause_testcase> cases(std::initializer_list<clause_testcase> il)
    {
      return std::vector<clause_testcase>(il);
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
  
      kyaml::context &ctx()
      {
        return d_ctx.get();
      }

      // tests:
      void test_match()
      {
        bool const expected = GetParam().result;

        null_builder b;
        EXPECT_EQ(expected, clause().parse(b));
      }
      
      void test_advance()
      {
        size_t const expected = GetParam().consumed;
        null_builder b;
        clause().parse(b);
        EXPECT_EQ(expected, ctx().stream().pos());
      }
       
    private:
      context_wrap d_ctx;
      clause_t d_clause;
    };
  }
}

namespace std
{
  inline ostream &operator<<(ostream &o, kyaml::test::clause_testcase const &tc)
  {
    return o << "\n"
             << "input = \"" << tc.input << "\"\n"
             << "indent_level = " << tc.indent_level << "\n"
             << "blockflow = " << tc.blockflow << "\n"
             << "result = " << (tc.result ? "true" : "false") << "\n"
             << "consumed = " << tc.consumed << "\n";
  }
}

#define NAMED_CLAUSE_TEST(name, clausetype, values)                     \
  typedef kyaml::test::clause_test<clausetype> name;                    \
                                                                        \
  TEST_P(name, match)                                                   \
  {                                                                     \
    test_match();                                                       \
  }                                                                     \
                                                                        \
  TEST_P(name, advance)                                                 \
  {                                                                     \
    test_advance();                                                     \
  }                                                                     \
                                                                        \
  INSTANTIATE_TEST_SUITE_P(tests_##name,                                 \
                          name,                                         \
                          testing::ValuesIn(values));

#define CLAUSE_TEST(clausetype, values)                                 \
  NAMED_CLAUSE_TEST(test_##clausetype, clausetype, values)

#endif // CLAUSE_TEST_HH

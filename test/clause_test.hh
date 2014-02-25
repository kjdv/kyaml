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
                   unsigned indent_level = 0,
                   kyaml::clauses::context::blockflow_t bf = kyaml::clauses::context::NA) :
        d_sstream(s),
        d_stream(d_sstream),
        d_ctx(d_stream, indent_level, bf)
      {}

      kyaml::clauses::context const &get() const
      {
        return d_ctx;
      }
      
      kyaml::clauses::context &get()
      {
        return d_ctx;
      }
    private:
      std::stringstream d_sstream;
      char_stream d_stream;
      kyaml::clauses::context d_ctx;
    };

    struct clause_testcase
    {
      std::string input;
      unsigned indent_level;
      kyaml::clauses::context::blockflow_t blockflow;
      
      bool const result;
      unsigned const consumed;
      
      std::string const value;
    };

    class clause_builder
    {
    public:
      clause_builder(std::string const &input, bool result) :
        d_input(input),
        d_indent_level(0),
        d_blockflow(kyaml::clauses::context::NA),
        d_result(result),
        d_consumed(input.size())
      {}

      clause_builder &with_indent_level(unsigned l)
      {
        d_indent_level = l;
        return *this;
      }
      
      clause_builder &with_blockflow(kyaml::clauses::context::blockflow_t bf)
      {
        d_blockflow = bf;
        return *this;
      }

      clause_builder &with_consumed(unsigned n)
      {
        d_consumed = n;
        return *this;
      }

      clause_builder &with_value(std::string const &v)
      {
        d_value = v;
        return *this;
      }      

      clause_testcase build()
      {
        clause_testcase result = {d_input,
                                  d_indent_level,
                                  d_blockflow,
                                  d_result,
                                  d_consumed,
                                  d_value};
        return result;
      }
    private:
      std::string d_input;
      unsigned d_indent_level;
      kyaml::clauses::context::blockflow_t d_blockflow;
      
      bool d_result;
      unsigned d_consumed;
      
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

    template <typename value_t>
    std::string as_string(value_t const &v)
    {
      return std::string(v);
    }
    
    template<>
    inline std::string as_string(char32_t const &c)
    {
      std::string result;
      kyaml::append_utf8(result, c);
      return result;
    }

    template<>
    inline std::string as_string(kyaml::clauses::void_result const &v)
    {
      return std::string();
    }

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
          std::string const &expected = GetParam().value;
          EXPECT_EQ(expected, as_string(clause().value()));
        }
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
             << "consumed = " << tc.consumed << "\n"
             << "value = " << tc.value << "\n";
  }
}

#define CLAUSE_TEST(clausetype, values)                                 \
  typedef kyaml::test::clause_test<clausetype> test_##clausetype;       \
                                                                        \
  TEST_P(test_##clausetype, match)                                      \
  {                                                                     \
    test_match();                                                       \
  }                                                                     \
                                                                        \
  TEST_P(test_##clausetype, advance)                                    \
  {                                                                     \
    test_advance();                                                     \
  }                                                                     \
  TEST_P(test_##clausetype, value)                                      \
  {                                                                     \
    test_value();                                                       \
  }                                                                     \
                                                                        \
  INSTANTIATE_TEST_CASE_P(tests_##clausetype,                           \
                          test_##clausetype,                            \
                          testing::ValuesIn(values));


#endif // CLAUSE_TEST_HH

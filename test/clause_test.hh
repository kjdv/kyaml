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

    class string_document_builder : public document_builder
    {
    public:
      void add(char const *tag, void_item const &v) override
      {}

      void add(char const *tag, std::string const &s) override
      {
        append_utf8(d_str, s);
      }

      document_builder::child_t child() override
      {
        return child_t(new string_document_builder);
      }

      void add(char const *tag, document_builder::child_t c) override
      {
        d_str.append(dynamic_cast<string_document_builder *>(c.get())->d_str);
      }

      std::string const &result() const
      {
        return d_str;
      }

    private:
      std::string d_str;
    };

    template <typename value_t>
    struct clause_testcase
    {
      std::string input;
      unsigned indent_level;
      kyaml::clauses::context::blockflow_t blockflow;
      
      bool const result;
      unsigned const consumed;
      
      value_t const value;
    };

    template <typename value_t>
    class testcase_builder
    {
    public:
      testcase_builder(std::string const &input, bool result) :
        d_input(input),
        d_indent_level(0),
        d_blockflow(kyaml::clauses::context::NA),
        d_result(result),
        d_consumed(input.size())
      {}

      testcase_builder<value_t> &with_indent_level(unsigned l)
      {
        d_indent_level = l;
        return *this;
      }
      
      testcase_builder<value_t> &with_blockflow(kyaml::clauses::context::blockflow_t bf)
      {
        d_blockflow = bf;
        return *this;
      }

      testcase_builder<value_t> &with_consumed(unsigned n)
      {
        d_consumed = n;
        return *this;
      }

      testcase_builder<value_t> &with_value(value_t const &v)
      {
        d_value = v;
        return *this;
      }      

      clause_testcase<value_t> build()
      {
        clause_testcase<value_t> result = {d_input,
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
      
      value_t d_value;
    };

    template <typename value_t>
    inline std::vector<clause_testcase<value_t> > cases(std::initializer_list<clause_testcase<value_t> > il)
    {
      return std::vector<clause_testcase<value_t> >(il);
    }

    template <typename clause_t>
    void setup(clause_t &)
    {}
    
    template <typename clause_t>
    void teardown(clause_t &)
    {}

    template <typename clause_t>
    class clause_test : public testing::TestWithParam<clause_testcase
                                                      <typename clause_t::value_t> >
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
          auto expected = GetParam().value;
          EXPECT_EQ(expected, clause().value());
        }
      }

      // tests:
      void test_pmatch()
      {
        bool const expected = GetParam().result;

        string_document_builder b;
        EXPECT_EQ(expected, clause().parse(b));
      }
      
      void test_padvance()
      {
        size_t const expected = GetParam().consumed;
        string_document_builder b;
        clause().parse(b);
        EXPECT_EQ(expected, ctx().stream().pos());
      }
      
      void test_pvalue()
      {
        if(GetParam().result)
        {
          string_document_builder b;
          clause().parse(b);
          auto expected = GetParam().value;
          EXPECT_EQ(expected, b.result());
        }
      }  
      
    private:
      clause_testcase<typename clause_t::value_t> const &GetParam() const
      {
        return testing::TestWithParam<clause_testcase
                                      <typename clause_t::value_t> >::GetParam();
      }


      context_wrap d_ctx;
      clause_t d_clause;
    };
  }
}

namespace std
{
  template <typename value_t>
  inline ostream &operator<<(ostream &o, kyaml::test::clause_testcase<value_t> const &tc)
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
  TEST_P(name, value)                                                   \
  {                                                                     \
    test_value();                                                       \
  }                                                                     \
                                                                        \
  INSTANTIATE_TEST_CASE_P(tests_##name,                                 \
                          name,                                         \
                          testing::ValuesIn(values));

#define CLAUSE_TEST(clausetype, values)                                 \
  NAMED_CLAUSE_TEST(test_##clausetype, clausetype, values)

#define PNAMED_CLAUSE_TEST(name, clausetype, values)                     \
  typedef kyaml::test::clause_test<clausetype> name;                    \
                                                                        \
  TEST_P(name, match)                                                   \
  {                                                                     \
    test_pmatch();                                                       \
  }                                                                     \
                                                                        \
  TEST_P(name, advance)                                                 \
  {                                                                     \
    test_padvance();                                                     \
  }                                                                     \
  TEST_P(name, value)                                                   \
  {                                                                     \
    test_pvalue();                                                       \
  }                                                                     \
                                                                        \
  INSTANTIATE_TEST_CASE_P(tests_##name,                                 \
                          name,                                         \
                          testing::ValuesIn(values));

#define PCLAUSE_TEST(clausetype, values)                                 \
  PNAMED_CLAUSE_TEST(parsetest_##clausetype, clausetype, values)


#endif // CLAUSE_TEST_HH

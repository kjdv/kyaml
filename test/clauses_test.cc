#include "clauses.hh"
#include <sstream>
#include <gtest/gtest.h>

using namespace std;
using namespace kyaml;
using namespace kyaml::clauses;

template <typename clause_t, typename value_t>
class clause_testcase
{
public:
  clause_testcase(string const &input) :
    d_str(input),
    d_ps(d_str),
    d_clause(d_ps)
  {}

  bool try_clause()
  {
    return d_clause.try_clause();
  }

  value_t consume()
  {
    return d_clause.consume();
  }

private:
  stringstream d_str;
  peekable_stream d_ps;

  clause_t d_clause;
};

#define CLAUSE_TEST(clause, value, name, input, expected) \
  TEST(clause, try_##name)                                \
  {                                                       \
    clause_testcase<clause, value> c(input);              \
    EXPECT_TRUE(c.try_clause());                          \
  }                                                       \
  TEST(clause, consume_##name)                            \
  {                                                       \
    clause_testcase<clause, value> c(input);              \
    EXPECT_EQ(expected, c.consume());                     \
  }

#define NEG_CLAUSE_TEST(clause, value, name, input) \
  TEST(clause, negative_try_##name)                 \
  {                                                 \
    clause_testcase<clause, value> c(input);        \
    EXPECT_FALSE(c.try_clause());                   \
  }

CLAUSE_TEST(printable, char, simple, "a", 'a')
CLAUSE_TEST(printable, char, tab, "\t", '\t')
CLAUSE_TEST(printable, char, newline, "\n", '\n')
NEG_CLAUSE_TEST(printable, char, nonprintable, "\x10");

CLAUSE_TEST(json, char, simple, "a", 'a')
CLAUSE_TEST(json, char, tab, "\t", '\t')
NEG_CLAUSE_TEST(json, char, newline, "\n")

CLAUSE_TEST(sequence_entry, char, positive, "-", '-')
NEG_CLAUSE_TEST(sequence_entry, char, negative, "a");

CLAUSE_TEST(mapping_key, char, positive, "?", '?')
NEG_CLAUSE_TEST(mapping_key, char, negative, "a");

CLAUSE_TEST(mapping_value, char, positive, ":", ':')
NEG_CLAUSE_TEST(mapping_value, char, negative, "a");

CLAUSE_TEST(collect_entry, char, positive, ",", ',')
NEG_CLAUSE_TEST(collect_entry, char, negative, "a");

CLAUSE_TEST(sequence_start, char, positive, "[", '[')
NEG_CLAUSE_TEST(sequence_start, char, negative, "a");

CLAUSE_TEST(sequence_end, char, positive, "]", ']')
NEG_CLAUSE_TEST(sequence_end, char, negative, "a");

CLAUSE_TEST(mapping_start, char, positive, "{", '{')
NEG_CLAUSE_TEST(mapping_start, char, negative, "a");

CLAUSE_TEST(mapping_end, char, positive, "}", '}')
NEG_CLAUSE_TEST(mapping_end, char, negative, "a");

CLAUSE_TEST(comment, char, positive, "#", '#')
NEG_CLAUSE_TEST(comment, char, negative, "a");

CLAUSE_TEST(anchor, char, positive, "&", '&')
NEG_CLAUSE_TEST(anchor, char, negative, "a");

CLAUSE_TEST(alias, char, positive, "*", '*')
NEG_CLAUSE_TEST(alias, char, negative, "a");

CLAUSE_TEST(tag, char, positive, "!", '!')
NEG_CLAUSE_TEST(tag, char, negative, "a");

CLAUSE_TEST(literal, char, positive, "|", '|')
NEG_CLAUSE_TEST(literal, char, negative, "a");

CLAUSE_TEST(folded, char, positive, ">", '>')
NEG_CLAUSE_TEST(folded, char, negative, "a");

CLAUSE_TEST(single_quote, char, positive, "'", '\'')
NEG_CLAUSE_TEST(single_quote, char, negative, "a");

CLAUSE_TEST(double_quote, char, positive, "\"", '"')
NEG_CLAUSE_TEST(double_quote, char, negative, "a");

CLAUSE_TEST(directive, char, positive, "%", '%')
NEG_CLAUSE_TEST(directive, char, negative, "a");

CLAUSE_TEST(reserved, char, at, "@", '@')
CLAUSE_TEST(reserved, char, backtick, "`", '`')
NEG_CLAUSE_TEST(reserved, char, negative, "a");

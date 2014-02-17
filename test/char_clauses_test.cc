#include "char_clauses.hh"
#include <sstream>
#include <algorithm>
#include <vector>
#include <initializer_list>
#include <gtest/gtest.h>

using namespace std;
using namespace kyaml;
using namespace kyaml::clauses;

namespace 
{
  // count the number of non-continuation bytes in the string
  inline size_t non_continuation(string const &s)
  {
    return count_if(s.begin(), s.end(), 
                    [](char c) 
                    { 
                      return (c & 0xc0) != 0x80;
                    });
  }

  inline vector<string> values(initializer_list<string> il)
  {
    return vector<string>(il);
  }
}

template <typename clause_t>
class char_clause_test : public testing::TestWithParam<string>
{
public:
  char_clause_test() :
    d_input(GetParam()),
    d_stream(d_input),
    d_cstream(d_stream),
    d_clause(d_cstream)
  {}

  bool try_clause()
  {
    return d_clause.try_clause();
  }

  typename clause_t::value_t value() const
  {
    return d_clause.value();
  }

  string const &input() const
  {
    return d_input;
  }

  char_stream const &stream() const
  {
    return d_cstream;
  }

  // tests
  
  // (positive)
  void test_match()
  {
    EXPECT_TRUE(try_clause());
  }
  
  void test_advance()
  {
    try_clause();
    EXPECT_EQ(non_continuation(input()), stream().pos());
  }

  void test_value()
  {
    try_clause();

    string actual;
    append_utf8(actual, value());

    EXPECT_EQ(input(), actual);
  }

  // (negative)
  void test_notmatch()
  {
    EXPECT_FALSE(try_clause());
  }
  
  void test_noadvance()
  {
    try_clause();
    EXPECT_EQ(0, stream().pos());
  }
private:
  string d_input;
  stringstream d_stream;
  char_stream d_cstream;
  clause_t d_clause;
};

// GTest has value-parameterized tests, GTest has type-parameterized tests, but GTest has no 
// type-and-value parameterized tests, some hackery required.

#define TP_POS_TEST(clausetype, values)                         \
  typedef char_clause_test<clausetype> positive_##clausetype;   \
                                                                \
  TEST_P(positive_##clausetype, match)                          \
  {                                                             \
    test_match();                                               \
  }                                                             \
  TEST_P(positive_##clausetype, advance)                        \
  {                                                             \
    test_advance();                                             \
  }                                                             \
  TEST_P(positive_##clausetype, value)                          \
  {                                                             \
    test_match();                                               \
  }                                                             \
  INSTANTIATE_TEST_CASE_P(char_test,                            \
                          positive_##clausetype,                \
                          testing::ValuesIn(values));

#define TP_NEG_TEST(clausetype, values)                           \
  typedef char_clause_test<clausetype> negative_##clausetype;     \
                                                                  \
  TEST_P(negative_##clausetype, match)                            \
  {                                                               \
    test_notmatch();                                              \
  }                                                               \
  TEST_P(negative_##clausetype, advance)                          \
  {                                                               \
    test_noadvance();                                             \
  }                                                               \
  INSTANTIATE_TEST_CASE_P(char_test,                              \
                          negative_##clausetype,                  \
                          testing::ValuesIn(values));

TP_POS_TEST(printable, values({"a", "\t", "\n", "\xd5\x82", "\xf0\x9d\x84\x8b"}))
TP_NEG_TEST(printable, values({"\x10", ""}))

TP_POS_TEST(json, values({"a", "t", "\xd5\x81"}))
TP_NEG_TEST(json, values({"\n", ""}))

TP_POS_TEST(sequence_entry, values({"-"}))
TP_NEG_TEST(sequence_entry, values({"a", ""}))

TP_POS_TEST(mapping_key, values({"?"}))
TP_NEG_TEST(mapping_key, values({"", "a"}))

TP_POS_TEST(mapping_value, values({":"}))
TP_NEG_TEST(mapping_value, values({"", "a"}))

TP_POS_TEST(collect_entry, values({","}))
TP_NEG_TEST(collect_entry, values({"", "a"}))

TP_POS_TEST(sequence_start, values({"["}))
TP_NEG_TEST(sequence_start, values({"", "a"}))

TP_POS_TEST(sequence_end, values({"]"}))
TP_NEG_TEST(sequence_end, values({"", "a"}))

TP_POS_TEST(mapping_start, values({"{"}))
TP_NEG_TEST(mapping_start, values({"", "a"}))

TP_POS_TEST(mapping_end, values({"}"}))
TP_NEG_TEST(mapping_end, values({"", "a"}))

TP_POS_TEST(comment, values({"#"}))
TP_NEG_TEST(comment, values({"", "a"}))

TP_POS_TEST(anchor, values({"&"}));
TP_NEG_TEST(anchor, values({"", "a"}))

TP_POS_TEST(alias, values({"*"}));
TP_NEG_TEST(alias, values({"", "a"}))

TP_POS_TEST(tag, values({"!"}))
TP_NEG_TEST(tag, values({"", "a"}))

TP_POS_TEST(literal, values({"|"}))
TP_NEG_TEST(literal, values({"", "a"}))

TP_POS_TEST(folded, values({">"}))
TP_NEG_TEST(folded, values({"", "a"}))

TP_POS_TEST(single_quote, values({"'"}))
TP_NEG_TEST(single_quote, values({"", "a"}))

TP_POS_TEST(double_quote, values({"\""}))
TP_NEG_TEST(double_quote, values({"", "a"}))

TP_POS_TEST(directive, values({"%"}))
TP_NEG_TEST(directive, values({"", "a"}))

TP_POS_TEST(reserved, values({"@", "`"}))
TP_NEG_TEST(reserved, values({"", "a"}))

TP_POS_TEST(indicator, values({"-", "?", ":", ",", "[", "]", "{", "}", "#",
                               "&", "*", "!", ">", "'", "\"", "%", "@", "`"}))
TP_NEG_TEST(indicator, values({"", "a"}))

TP_POS_TEST(flow_indicator, values({",", "[", "]", "{", "}"}))
TP_NEG_TEST(flow_indicator, values({"", "a"}))

TP_POS_TEST(line_feed, values({"\n"}))
TP_NEG_TEST(line_feed, values({"", "a"}))

TP_POS_TEST(carriage_return, values({"\r"}))
TP_NEG_TEST(carriage_return, values({"", "a"}))

TP_POS_TEST(break_char, values({"\n", "\r"}))
TP_NEG_TEST(break_char, values({"", "a"}))

TP_POS_TEST(non_break_char, values({"a"}))
TP_NEG_TEST(non_break_char, values({"", "\r", "\n", "\xfe\xff"}))

TP_POS_TEST(line_break, values({"\n", "\r", "\r\n"}))
TP_NEG_TEST(line_break, values({"", "a"}));

TP_POS_TEST(space, values({" "}))
TP_NEG_TEST(space, values({"", "a"}))

TP_POS_TEST(tab, values({"\t"}))
TP_NEG_TEST(tab, values({"", "a"}))

TP_POS_TEST(white, values({" ", "\t"}))
TP_NEG_TEST(white, values({"", "a"}))

TP_POS_TEST(non_white_char, values({"a"}))
TP_NEG_TEST(non_white_char, values({"", " ", "\t", "\r", "\n"}))

TP_POS_TEST(dec_digit_char, values({"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"}))
TP_NEG_TEST(dec_digit_char, values({"", "a"}))

TP_POS_TEST(hex_digit_char, values({"0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
                                    "a", "b", "c", "d", "e", "f",
                                    "A", "B", "C", "D", "E", "F"}))
TP_NEG_TEST(hex_digit_char, values({"", "g", "G"}))

TP_POS_TEST(ascii_letter, values({"a", "b", "k", "z", "A", "L", "M", "Z"}))
TP_NEG_TEST(ascii_letter, values({"", "0", "$"}))

TP_POS_TEST(word_char, values({"2", "a", "Z", "-"}))
TP_NEG_TEST(word_char, values({"", "$"}))

TP_POS_TEST(uri_char, values({"%01", "%ab", "a", "C", "1", "-", ";", "/", "?", ":", "@", 
                              "&", "=", "+", "$", ",", "_", ".", "!", "~", "*", "(", ")", 
                              "[", "]", "#"}))

TP_POS_TEST(tag_char, values({"%01", "%ab", "a", "C", "1", "-", ";", "/", "?", ":", "@", 
                              "&", "=", "+", "$", "_", ".", "~", "*", "#"}))
TP_NEG_TEST(tag_char, values({"", "!", ",", "[", "]", "{", "}"}))

TP_POS_TEST(escape, values({"\\"}))
TP_NEG_TEST(escape, values({"", "a"}))

TP_POS_TEST(esc_null, values({"0"}))
TP_POS_TEST(esc_bell, values({"a"}))
TP_POS_TEST(esc_backspace, values({"b"}))
TP_POS_TEST(esc_linefeed, values({"n"}))
TP_POS_TEST(esc_htab, values({"t"}))
TP_POS_TEST(esc_vtab, values({"v"}))
TP_POS_TEST(esc_form_feed, values({"f"}))
TP_POS_TEST(esc_carriage_return, values({"r"}))
TP_POS_TEST(esc_escape, values({"e"}))
TP_POS_TEST(esc_space, values({" "}))
TP_POS_TEST(esc_dquote, values({"\""}))
TP_POS_TEST(esc_slash, values({"/"}))
TP_POS_TEST(esc_bslash, values({"\\"}))
TP_POS_TEST(esc_next_line, values({"N"}))
TP_POS_TEST(esc_non_break_space, values({"_"}))
TP_POS_TEST(esc_line_separator, values({"L"}))
TP_POS_TEST(esc_paragraph_separator, values({"P"}))

TP_POS_TEST(esc_unicode_8b, values({"x01"}))
TP_NEG_TEST(esc_unicode_8b, values({"x0G"}))

TP_POS_TEST(esc_unicode_16b, values({"u0123"}))
TP_NEG_TEST(esc_unicode_16b, values({"u0G"}))

TP_POS_TEST(esc_unicode_32b, values({"U0123abcd"}))
TP_NEG_TEST(esc_unicode_32b, values({"U0G"}))

TP_POS_TEST(esc_char, values({"\\0", "\\a", "\\b", "\\n", "\\t", "\\v", "\\f", "\\r", "\\e",
                              "\\ ", "\\\\", "\\/", "\\\"", "\\N", "\\_", "\\L", "\\P", 
                              "\\x01", "\\u1234", "\\U12345678"}))

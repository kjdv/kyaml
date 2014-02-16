#include "char_clauses.hh"
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
    d_cs(d_str),
    d_clause(d_cs)
  {}

  bool try_clause()
  {
    return d_clause.try_clause();
  }

  value_t value()
  {
    return d_clause.value();
  }

  char_stream &stream()
  {
    return d_cs;
  }

private:
  stringstream d_str;
  char_stream d_cs;

  clause_t d_clause;
};

#define CHAR_CLAUSE_TEST(clause, rvalue, name, input, expected) \
  TEST(clause, try_##name)                                      \
  {                                                             \
    clause_testcase<clause, rvalue> c(input);                   \
    EXPECT_TRUE(c.try_clause());                                \
  }                                                             \
  TEST(clause, advance_##name)                                  \
  {                                                             \
    clause_testcase<clause, rvalue> c(input);                   \
    c.try_clause();                                             \
    EXPECT_EQ(string(input).size(), c.stream().pos());          \
  }                                                             \
  TEST(clause, consume_##name)                                  \
  {                                                             \
    clause_testcase<clause, rvalue> c(input);                   \
    c.try_clause();                                             \
    EXPECT_EQ(expected, c.value());                             \
  }

#define NEG_CHAR_CLAUSE_TEST(clause, rvalue, name, input)        \
  TEST(clause, negative_try_##name)                              \
  {                                                              \
    clause_testcase<clause, rvalue> c(input);                    \
    EXPECT_FALSE(c.try_clause());                                \
  }                                                              \
  TEST(clause, advance_##name)                                   \
  {                                                              \
    clause_testcase<clause, rvalue> c(input);                    \
    c.try_clause();                                              \
    EXPECT_EQ(0, c.stream().pos());                              \
  }                                                              

CHAR_CLAUSE_TEST(printable, string, simple, "a", "a")
CHAR_CLAUSE_TEST(printable, string, tab, "\t", "\t")
CHAR_CLAUSE_TEST(printable, string, newline, "\n", "\n")
CHAR_CLAUSE_TEST(printable, string, mutlibyte1, "\xd5\x81", "\xd5\x81")
CHAR_CLAUSE_TEST(printable, string, mutlibyte2, "\xf0\x9d\x84\x8b", "\xf0\x9d\x84\x8b");
NEG_CHAR_CLAUSE_TEST(printable, string, nonprintable, "\x10");

CHAR_CLAUSE_TEST(json, string, simple, "a", "a")
CHAR_CLAUSE_TEST(json, string, tab, "\t", "\t")
CHAR_CLAUSE_TEST(json, string, mutlibyte, "\xd5\x81", "\xd5\x81")
NEG_CHAR_CLAUSE_TEST(json, string, newline, "\n")

CHAR_CLAUSE_TEST(sequence_entry, char, positive, "-", '-')
NEG_CHAR_CLAUSE_TEST(sequence_entry, char, negative, "a");

CHAR_CLAUSE_TEST(mapping_key, char, positive, "?", '?')
NEG_CHAR_CLAUSE_TEST(mapping_key, char, negative, "a");

CHAR_CLAUSE_TEST(mapping_value, char, positive, ":", ':')
NEG_CHAR_CLAUSE_TEST(mapping_value, char, negative, "a");

CHAR_CLAUSE_TEST(collect_entry, char, positive, ",", ',')
NEG_CHAR_CLAUSE_TEST(collect_entry, char, negative, "a");

CHAR_CLAUSE_TEST(sequence_start, char, positive, "[", '[')
NEG_CHAR_CLAUSE_TEST(sequence_start, char, negative, "a");

CHAR_CLAUSE_TEST(sequence_end, char, positive, "]", ']')
NEG_CHAR_CLAUSE_TEST(sequence_end, char, negative, "a");

CHAR_CLAUSE_TEST(mapping_start, char, positive, "{", '{')
NEG_CHAR_CLAUSE_TEST(mapping_start, char, negative, "a");

CHAR_CLAUSE_TEST(mapping_end, char, positive, "}", '}')
NEG_CHAR_CLAUSE_TEST(mapping_end, char, negative, "a");

CHAR_CLAUSE_TEST(comment, char, positive, "#", '#')
NEG_CHAR_CLAUSE_TEST(comment, char, negative, "a");

CHAR_CLAUSE_TEST(anchor, char, positive, "&", '&')
NEG_CHAR_CLAUSE_TEST(anchor, char, negative, "a");

CHAR_CLAUSE_TEST(alias, char, positive, "*", '*')
NEG_CHAR_CLAUSE_TEST(alias, char, negative, "a");

CHAR_CLAUSE_TEST(tag, char, positive, "!", '!')
NEG_CHAR_CLAUSE_TEST(tag, char, negative, "a");

CHAR_CLAUSE_TEST(literal, char, positive, "|", '|')
NEG_CHAR_CLAUSE_TEST(literal, char, negative, "a");

CHAR_CLAUSE_TEST(folded, char, positive, ">", '>')
NEG_CHAR_CLAUSE_TEST(folded, char, negative, "a");

CHAR_CLAUSE_TEST(single_quote, char, positive, "'", '\'')
NEG_CHAR_CLAUSE_TEST(single_quote, char, negative, "a");

CHAR_CLAUSE_TEST(double_quote, char, positive, "\"", '"')
NEG_CHAR_CLAUSE_TEST(double_quote, char, negative, "a");

CHAR_CLAUSE_TEST(directive, char, positive, "%", '%')
NEG_CHAR_CLAUSE_TEST(directive, char, negative, "a");

CHAR_CLAUSE_TEST(reserved, char, at, "@", '@')
CHAR_CLAUSE_TEST(reserved, char, backtick, "`", '`')
NEG_CHAR_CLAUSE_TEST(reserved, char, negative, "a");

CHAR_CLAUSE_TEST(indicator, char, indicator1, "-", '-')
CHAR_CLAUSE_TEST(indicator, char, indicator2, "?", '?')
CHAR_CLAUSE_TEST(indicator, char, indicator3, ":", ':')
CHAR_CLAUSE_TEST(indicator, char, indicator4, ",", ',')
CHAR_CLAUSE_TEST(indicator, char, indicator5, "[", '[')
CHAR_CLAUSE_TEST(indicator, char, indicator6, "]", ']')
CHAR_CLAUSE_TEST(indicator, char, indicator7, "{", '{')
CHAR_CLAUSE_TEST(indicator, char, indicator8, "}", '}')
CHAR_CLAUSE_TEST(indicator, char, indicator9, "#", '#')
CHAR_CLAUSE_TEST(indicator, char, indicator10, "&", '&')
CHAR_CLAUSE_TEST(indicator, char, indicator11, "*", '*')
CHAR_CLAUSE_TEST(indicator, char, indicator12, "!", '!')
CHAR_CLAUSE_TEST(indicator, char, indicator13, ">", '>')
CHAR_CLAUSE_TEST(indicator, char, indicator14, "'", '\'')
CHAR_CLAUSE_TEST(indicator, char, indicator15, "\"", '"')
CHAR_CLAUSE_TEST(indicator, char, indicator16, "%", '%')
CHAR_CLAUSE_TEST(indicator, char, indicator17, "@", '@')
CHAR_CLAUSE_TEST(indicator, char, indicator18, "`", '`')

CHAR_CLAUSE_TEST(flow_indicator, char, flow_indicator1, ",", ',')
CHAR_CLAUSE_TEST(flow_indicator, char, flow_indicator2, "[", '[')
CHAR_CLAUSE_TEST(flow_indicator, char, flow_indicator3, "]", ']')
CHAR_CLAUSE_TEST(flow_indicator, char, flow_indicator4, "{", '{')
CHAR_CLAUSE_TEST(flow_indicator, char, flow_indicator5, "}", '}')

CHAR_CLAUSE_TEST(line_feed, char, positive, "\n", '\n')
NEG_CHAR_CLAUSE_TEST(line_feed, char, negative, "a");

CHAR_CLAUSE_TEST(carriage_return, char, positive, "\r", '\r')
NEG_CHAR_CLAUSE_TEST(carriage_return, char, negative, "a");

CHAR_CLAUSE_TEST(break_char, char, positive1, "\r", '\r')
CHAR_CLAUSE_TEST(break_char, char, positive2, "\n", '\n')
NEG_CHAR_CLAUSE_TEST(break_char, char, negative, "a");

CHAR_CLAUSE_TEST(non_break_char, string, positive, "a", "a")
NEG_CHAR_CLAUSE_TEST(non_break_char, string, negative1, "\n");
NEG_CHAR_CLAUSE_TEST(non_break_char, string, negative2, "\r");

CHAR_CLAUSE_TEST(line_break, string, windows, "\r\n", "\r\n");
CHAR_CLAUSE_TEST(line_break, string, mac, "\r", "\r");
CHAR_CLAUSE_TEST(line_break, string, unix, "\n", "\n");

CHAR_CLAUSE_TEST(space, char, positive, " ", ' ')
NEG_CHAR_CLAUSE_TEST(space, char, negative, "a");

CHAR_CLAUSE_TEST(tab, char, positive, "\t", '\t')
NEG_CHAR_CLAUSE_TEST(tab, char, negative, "a");

CHAR_CLAUSE_TEST(white, char, positive1, " ", ' ')
CHAR_CLAUSE_TEST(white, char, positive2, "\t", '\t')
NEG_CHAR_CLAUSE_TEST(white, char, negative, "a");

CHAR_CLAUSE_TEST(non_white_char, string, positive, "a", "a")
NEG_CHAR_CLAUSE_TEST(non_white_char, string, negative1, "\n");
NEG_CHAR_CLAUSE_TEST(non_white_char, string, negative2, "\r");
NEG_CHAR_CLAUSE_TEST(non_white_char, string, negative3, "\t");
NEG_CHAR_CLAUSE_TEST(non_white_char, string, negative4, " ");

CHAR_CLAUSE_TEST(dec_digit_char, char, positive, "1", '1')
NEG_CHAR_CLAUSE_TEST(dec_digit_char, char, negative, "a");

CHAR_CLAUSE_TEST(hex_digit_char, char, positive1, "1", '1')
CHAR_CLAUSE_TEST(hex_digit_char, char, positive2, "c", 'c')
CHAR_CLAUSE_TEST(hex_digit_char, char, positive3, "D", 'D')
NEG_CHAR_CLAUSE_TEST(hex_digit_char, char, negative, "g");

CHAR_CLAUSE_TEST(ascii_letter, char, positive1, "a", 'a')
CHAR_CLAUSE_TEST(ascii_letter, char, positive2, "B", 'B')
NEG_CHAR_CLAUSE_TEST(ascii_letter, char, negative, "2");

CHAR_CLAUSE_TEST(word_char, char, positive1, "2", '2')
CHAR_CLAUSE_TEST(word_char, char, positive2, "Z", 'Z')
CHAR_CLAUSE_TEST(word_char, char, positive3, "-", '-')
NEG_CHAR_CLAUSE_TEST(word_char, char, negative, "$");

CHAR_CLAUSE_TEST(uri_char, string, positive1, "%01", "%01");
CHAR_CLAUSE_TEST(uri_char, string, positive2, "%ab", "%ab");
CHAR_CLAUSE_TEST(uri_char, string, positive3, ";", ";");
CHAR_CLAUSE_TEST(uri_char, string, positive4, "/", "/");
CHAR_CLAUSE_TEST(uri_char, string, positive5, "?", "?");
CHAR_CLAUSE_TEST(uri_char, string, positive6, ":", ":");
CHAR_CLAUSE_TEST(uri_char, string, positive7, "@", "@");
CHAR_CLAUSE_TEST(uri_char, string, positive8, "&", "&");
CHAR_CLAUSE_TEST(uri_char, string, positive9, "=", "=");
CHAR_CLAUSE_TEST(uri_char, string, positive10, "+", "+");
CHAR_CLAUSE_TEST(uri_char, string, positive11, "$", "$");
CHAR_CLAUSE_TEST(uri_char, string, positive12, ",", ",");
CHAR_CLAUSE_TEST(uri_char, string, positive13, "_", "_");
CHAR_CLAUSE_TEST(uri_char, string, positive14, ".", ".");
CHAR_CLAUSE_TEST(uri_char, string, positive15, "!", "!");
CHAR_CLAUSE_TEST(uri_char, string, positive16, "~", "~");
CHAR_CLAUSE_TEST(uri_char, string, positive17, "*", "*");
CHAR_CLAUSE_TEST(uri_char, string, positive18, "(", "(");
CHAR_CLAUSE_TEST(uri_char, string, positive19, ")", ")");
CHAR_CLAUSE_TEST(uri_char, string, positive20, "[", "[");
CHAR_CLAUSE_TEST(uri_char, string, positive21, "]", "]");
CHAR_CLAUSE_TEST(uri_char, string, positive22, "#", "#");

CHAR_CLAUSE_TEST(tag_char, string, positive1, "%01", "%01");
CHAR_CLAUSE_TEST(tag_char, string, positive2, "%ab", "%ab");
CHAR_CLAUSE_TEST(tag_char, string, positive3, ";", ";");
CHAR_CLAUSE_TEST(tag_char, string, positive4, "/", "/");
CHAR_CLAUSE_TEST(tag_char, string, positive5, "?", "?");
CHAR_CLAUSE_TEST(tag_char, string, positive6, ":", ":");
CHAR_CLAUSE_TEST(tag_char, string, positive7, "@", "@");
CHAR_CLAUSE_TEST(tag_char, string, positive8, "&", "&");
CHAR_CLAUSE_TEST(tag_char, string, positive9, "=", "=");
CHAR_CLAUSE_TEST(tag_char, string, positive10, "+", "+");
CHAR_CLAUSE_TEST(tag_char, string, positive11, "$", "$");
CHAR_CLAUSE_TEST(tag_char, string, positive12, "_", "_");
CHAR_CLAUSE_TEST(tag_char, string, positive13, ".", ".");
CHAR_CLAUSE_TEST(tag_char, string, positive14, "~", "~");
CHAR_CLAUSE_TEST(tag_char, string, positive15, "*", "*");
CHAR_CLAUSE_TEST(tag_char, string, positive16, "(", "(");
CHAR_CLAUSE_TEST(tag_char, string, positive17, ")", ")");
CHAR_CLAUSE_TEST(tag_char, string, positive18, "#", "#");
NEG_CHAR_CLAUSE_TEST(tag_char, string, negative1, "!");
NEG_CHAR_CLAUSE_TEST(tag_char, string, negative2, ",");
NEG_CHAR_CLAUSE_TEST(tag_char, string, negative3, "[");
NEG_CHAR_CLAUSE_TEST(tag_char, string, negative4, "]");
NEG_CHAR_CLAUSE_TEST(tag_char, string, negative5, "{");
NEG_CHAR_CLAUSE_TEST(tag_char, string, negative6, "}");

CHAR_CLAUSE_TEST(escape, char, positive, "\\", '\\')
CHAR_CLAUSE_TEST(esc_null, char, positive, "0", '0')
CHAR_CLAUSE_TEST(esc_bell, char, positive, "a", 'a')
CHAR_CLAUSE_TEST(esc_backspace, char, positive, "b", 'b')
CHAR_CLAUSE_TEST(esc_linefeed, char, positive, "n", 'n')
CHAR_CLAUSE_TEST(esc_htab, char, positive, "t", 't')
CHAR_CLAUSE_TEST(esc_vtab, char, positive, "v", 'v')
CHAR_CLAUSE_TEST(esc_form_feed, char, positive, "f", 'f')
CHAR_CLAUSE_TEST(esc_carriage_return, char, positive, "r", 'r')
CHAR_CLAUSE_TEST(esc_escape, char, positive, "e", 'e')
CHAR_CLAUSE_TEST(esc_space, char, positive, " ", ' ')
CHAR_CLAUSE_TEST(esc_dquote, char, positive, "\"", '"')
CHAR_CLAUSE_TEST(esc_slash, char, positive, "/", '/')
CHAR_CLAUSE_TEST(esc_bslash, char, positive, "\\", '\\')
CHAR_CLAUSE_TEST(esc_next_line, char, positive, "N", 'N')
CHAR_CLAUSE_TEST(esc_non_break_space, char, positive, "_", '_')
CHAR_CLAUSE_TEST(esc_line_separator, char, positive, "L", 'L')
CHAR_CLAUSE_TEST(esc_paragraph_separator, char, positive, "P", 'P')

CHAR_CLAUSE_TEST(esc_unicode_8b, string, positive, "x01", "x01");
NEG_CHAR_CLAUSE_TEST(esc_unicode_8b, string, negative, "x0G");
CHAR_CLAUSE_TEST(esc_unicode_16b, string, positive, "u0123", "u0123");
NEG_CHAR_CLAUSE_TEST(esc_unicode_16b, string, negative, "u0G");
CHAR_CLAUSE_TEST(esc_unicode_32b, string, positive, "U0123abcd", "U0123abcd");
NEG_CHAR_CLAUSE_TEST(esc_unicode_32b, string, negative, "U0G");

CHAR_CLAUSE_TEST(esc_char, string, positive0, "\\0", "\\0");
CHAR_CLAUSE_TEST(esc_char, string, positive1, "\\a", "\\a");
CHAR_CLAUSE_TEST(esc_char, string, positive2, "\\b", "\\b");
CHAR_CLAUSE_TEST(esc_char, string, positive3, "\\n", "\\n");
CHAR_CLAUSE_TEST(esc_char, string, positive4, "\\t", "\\t");
CHAR_CLAUSE_TEST(esc_char, string, positive5, "\\v", "\\v");
CHAR_CLAUSE_TEST(esc_char, string, positive6, "\\f", "\\f");
CHAR_CLAUSE_TEST(esc_char, string, positive7, "\\r", "\\r");
CHAR_CLAUSE_TEST(esc_char, string, positive8, "\\e", "\\e");
CHAR_CLAUSE_TEST(esc_char, string, positive9, "\\ ", "\\ ");
CHAR_CLAUSE_TEST(esc_char, string, positive10, "\\\\", "\\\\");
CHAR_CLAUSE_TEST(esc_char, string, positive11, "\\/", "\\/");
CHAR_CLAUSE_TEST(esc_char, string, positive12, "\\\"", "\\\"");
CHAR_CLAUSE_TEST(esc_char, string, positive13, "\\N", "\\N");
CHAR_CLAUSE_TEST(esc_char, string, positive14, "\\_", "\\_");
CHAR_CLAUSE_TEST(esc_char, string, positive15, "\\L", "\\L");
CHAR_CLAUSE_TEST(esc_char, string, positive16, "\\P", "\\P");
CHAR_CLAUSE_TEST(esc_char, string, positive17, "\\x01", "\\x01");
CHAR_CLAUSE_TEST(esc_char, string, positive18, "\\u1234", "\\u1234");
CHAR_CLAUSE_TEST(esc_char, string, positive19, "\\U12345678", "\\U12345678");

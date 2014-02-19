#include "char_clauses.hh"
#include "clause_test.hh"
#include <algorithm>
#include <vector>
#include <initializer_list>
#include <gtest/gtest.h>

using namespace std;
using namespace kyaml::test;
using namespace kyaml::clauses;

namespace 
{
  // count the number of non-continuation bytes in the string
  inline unsigned non_continuation(string const &s)
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

  clause_testcase tc(string const &input, bool result)
  {
    clause_testcase t =
    {
      input,
      0,
      context::NA,
      result,
      result ? non_continuation(input) : 0,
      input,
    };
    return t;
  }

  vector<clause_testcase> tvalues(vector<string> const &pos,
                                  vector<string> const &neg)
  {
    vector<clause_testcase> rv; rv.reserve(pos.size() + neg.size());
    for(auto item : pos)
      rv.push_back(tc(item, true));
    for(auto item : neg)
      rv.push_back(tc(item, false));
    return rv;
  }
}

#define CHAR_CLAUSE_TEST(clausetype, positive, negative)   \
  CLAUSE_TEST(clausetype, tvalues(positive, negative))

CHAR_CLAUSE_TEST(printable, 
                 values({"a", "\t", "\n", "\xd5\x82", "\xf0\x9d\x84\x8b"}),
                 values({"\x10", ""}))

CHAR_CLAUSE_TEST(json, 
                 values({"a", "t", "\xd5\x81"}),
                 values({"\n", ""}))

CHAR_CLAUSE_TEST(sequence_entry, 
                 values({"-"}),
                 values({"a", ""}))

CHAR_CLAUSE_TEST(mapping_key, 
                 values({"?"}),
                 values({"", "a"}))

CHAR_CLAUSE_TEST(mapping_value, 
                 values({":"}),
                 values({"", "a"}))

CHAR_CLAUSE_TEST(collect_entry, 
                 values({","}),
                 values({"", "a"}))

CHAR_CLAUSE_TEST(sequence_start, 
                 values({"["}),
                 values({"", "a"}))

CHAR_CLAUSE_TEST(sequence_end, 
                 values({"]"}),
                 values({"", "a"}))

CHAR_CLAUSE_TEST(mapping_start, 
                 values({"{"}),
                 values({"", "a"}))

CHAR_CLAUSE_TEST(mapping_end, 
                 values({"}"}),
                 values({"", "a"}))

CHAR_CLAUSE_TEST(comment, 
                 values({"#"}),
                 values({"", "a"}))

CHAR_CLAUSE_TEST(anchor, 
                 values({"&"}),
                 values({"", "a"}))

CHAR_CLAUSE_TEST(alias, 
                 values({"*"}),
                 values({"", "a"}))

CHAR_CLAUSE_TEST(tag, 
                 values({"!"}),
                 values({"", "a"}))

CHAR_CLAUSE_TEST(literal, 
                 values({"|"}),
                 values({"", "a"}))

CHAR_CLAUSE_TEST(folded, 
                 values({">"}),
                 values({"", "a"}))

CHAR_CLAUSE_TEST(single_quote, 
                 values({"'"}),
                 values({"", "a"}))

CHAR_CLAUSE_TEST(double_quote, 
                 values({"\""}),
                 values({"", "a"}))

CHAR_CLAUSE_TEST(directive, 
                 values({"%"}),
                 values({"", "a"}))

CHAR_CLAUSE_TEST(reserved, 
                 values({"@", "`"}),
                 values({"", "a"}))

CHAR_CLAUSE_TEST(indicator, 
                 values({"-", "?", ":", ",", "[", "]", "{", "}", "#",
                         "&", "*", "!", ">", "'", "\"", "%", "@", "`"}),
                 values({"", "a"}))

CHAR_CLAUSE_TEST(flow_indicator, 
                 values({",", "[", "]", "{", "}"}),
                 values({"", "a"}))

CHAR_CLAUSE_TEST(line_feed, 
                 values({"\n"}),
                 values({"", "a"}))

CHAR_CLAUSE_TEST(carriage_return, 
                 values({"\r"}),
                 values({"", "a"}))

CHAR_CLAUSE_TEST(break_char, 
                 values({"\n", "\r"}),
                 values({"", "a"}))

CHAR_CLAUSE_TEST(non_break_char, 
                 values({"a"}),
                 values({"", "\r", "\n", "\xfe\xff"}))

CHAR_CLAUSE_TEST(line_break, 
                 values({"\n", "\r", "\r\n"}),
                 values({"", "a"}));

CHAR_CLAUSE_TEST(space, 
                 values({" "}),
                 values({"", "a"}))

CHAR_CLAUSE_TEST(tab, 
                 values({"\t"}),
                 values({"", "a"}))

CHAR_CLAUSE_TEST(white, 
                 values({" ", "\t"}),
                 values({"", "a"}))

CHAR_CLAUSE_TEST(non_white_char, 
                 values({"a"}),
                 values({"", " ", "\t", "\r", "\n"}))

CHAR_CLAUSE_TEST(dec_digit_char, 
                 values({"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"}),
                 values({"", "a"}))

CHAR_CLAUSE_TEST(hex_digit_char, 
                 values({"0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
                         "a", "b", "c", "d", "e", "f",
                         "A", "B", "C", "D", "E", "F"}),
                 values({"", "g", "G"}))

CHAR_CLAUSE_TEST(ascii_letter, 
                 values({"a", "b", "k", "z", "A", "L", "M", "Z"}),
                 values({"", "0", "$"}))

CHAR_CLAUSE_TEST(word_char, 
                 values({"2", "a", "Z", "-"}),
                 values({"", "$"}))

CHAR_CLAUSE_TEST(uri_char, 
                 values({"%01", "%ab", "a", "C", "1", "-", ";", "/", "?", ":", "@", 
                         "&", "=", "+", "$", ",", "_", ".", "!", "~", "*", "(", ")", 
                         "[", "]", "#"}),
                 values({""}))

CHAR_CLAUSE_TEST(tag_char,
                 values({"%01", "%ab", "a", "C", "1", "-", ";", "/", "?", ":", "@", 
                         "&", "=", "+", "$", "_", ".", "~", "*", "#"}),
                 values({"", "!", ",", "[", "]", "{", "}"}))
 
CHAR_CLAUSE_TEST(escape, 
                 values({"\\"}),
                 values({"", "a"}))

CHAR_CLAUSE_TEST(esc_null, 
                 values({"0"}),
                 values({}))

CHAR_CLAUSE_TEST(esc_bell, 
                 values({"a"}),
                 values({}))
CHAR_CLAUSE_TEST(esc_backspace, 
                 values({"b"}),
                 values({}))

CHAR_CLAUSE_TEST(esc_linefeed, 
                 values({"n"}),
                 values({}))

CHAR_CLAUSE_TEST(esc_htab, 
                 values({"t"}),
                 values({}))

CHAR_CLAUSE_TEST(esc_vtab, 
                 values({"v"}),
                 values({}))

CHAR_CLAUSE_TEST(esc_form_feed, 
                 values({"f"}),
                 values({}))

CHAR_CLAUSE_TEST(esc_carriage_return, 
                 values({"r"}),
                 values({}))

CHAR_CLAUSE_TEST(esc_escape, 
                 values({"e"}),
                 values({}))

CHAR_CLAUSE_TEST(esc_space, 
                 values({" "}),
                 values({}))

CHAR_CLAUSE_TEST(esc_dquote, 
                 values({"\""}),
                 values({}))

CHAR_CLAUSE_TEST(esc_slash, 
                 values({"/"}),
                 values({}))

CHAR_CLAUSE_TEST(esc_bslash, 
                 values({"\\"}),
                 values({}))

CHAR_CLAUSE_TEST(esc_next_line, 
                 values({"N"}),
                 values({}))

CHAR_CLAUSE_TEST(esc_non_break_space, 
                 values({"_"}),
                 values({}))

CHAR_CLAUSE_TEST(esc_line_separator, 
                 values({"L"}),
                 values({}))

CHAR_CLAUSE_TEST(esc_paragraph_separator, 
                 values({"P"}),
                 values({}))

CHAR_CLAUSE_TEST(esc_unicode_8b, 
                 values({"x01"}),
                 values({"x0G"}))

CHAR_CLAUSE_TEST(esc_unicode_16b, 
                 values({"u0123"}),
                 values({"u0G"}))

CHAR_CLAUSE_TEST(esc_unicode_32b, 
                 values({"U0123abcd"}),
                 values({"U0G"}))

CHAR_CLAUSE_TEST(esc_char, 
                 values({"\\0", "\\a", "\\b", "\\n", "\\t", "\\v", "\\f", "\\r", "\\e",
                         "\\ ", "\\\\", "\\/", "\\\"", "\\N", "\\_", "\\L", "\\P", 
                       "\\x01", "\\u1234", "\\U12345678"}),
                 values({}))


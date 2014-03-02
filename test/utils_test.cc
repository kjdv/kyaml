#include "utils.hh"
#include <gtest/gtest.h>

using namespace std;
using namespace kyaml;

class utf8_test : public testing::TestWithParam<string>
{};

TEST_P(utf8_test, extract_and_append)
{
  string const &str = GetParam();

  stringstream stream(str); 
  char32_t ch;
  EXPECT_TRUE(extract_utf8(stream, ch));

  string out;
  append_utf8(out, ch);

  EXPECT_EQ(str, out);
}

string utf8_testcases[] =
{
  "a",
  "b",
  "\t",
  "\xd5\x82",
  "\xf0\x9d\x84\x8b",
};

INSTANTIATE_TEST_CASE_P(utf8_tests,
                        utf8_test,
                        testing::ValuesIn(utf8_testcases));

TEST(logger, one_item)
{
  stringstream str;
  logger<true> l(str);

  l("tag", 1);

  EXPECT_EQ("(tag): 1\n", str.str());
}

TEST(logger, two_items)
{
  stringstream str;
  logger<true> l(str);

  l("tag", "pi", 3.1459);

  EXPECT_EQ("(tag): pi 3.1459\n", str.str());
}

TEST(logger, zero_item)
{
  stringstream str;
  logger<true> l(str);

  l("tag");

  EXPECT_EQ("(tag):\n", str.str());
}

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

INSTANTIATE_TEST_SUITE_P(utf8_tests,
                        utf8_test,
                        testing::ValuesIn(utf8_testcases));

TEST(logger, one_item)
{
  stringstream str;
  logger<true> l("tag", str);

  l(1);

  EXPECT_EQ("(tag): 1\n", str.str());
}

TEST(logger, two_items)
{
  stringstream str;
  logger<true> l("tag", str);

  l("pi", 3.1459);

  EXPECT_EQ("(tag): pi 3.1459\n", str.str());
}

TEST(logger, zero_item)
{
  stringstream str;
  logger<true> l("tag", str);

  l();

  EXPECT_EQ("(tag):\n", str.str());
}

TEST(invalid_utf8, formatter)
{
  string sequence("\xfd\x82\x82\x82\x82\x82");
  invalid_utf8 e("message", sequence);

  EXPECT_EQ("message: \xFD \x82 \x82 \x82 \x82 \x82", string(e.what()));
}

struct nr_utf8bytes_testcase
{
  string input;
  size_t nr;
};

class nr_utf8bytes_test : public testing::TestWithParam<nr_utf8bytes_testcase>
{};

TEST_P(nr_utf8bytes_test, nr)
{
  uint8_t c = GetParam().input[0];
  EXPECT_EQ(GetParam().nr, nr_utf8bytes(c));
}

nr_utf8bytes_testcase nr_utf8bytes_testcases[] =
{
  {"a", 1},
  {"\xd5\x82", 2},
  {"\xe1\x82\x82", 3},
  {"\xf1\x82\x82\x82", 4},
  {"\xf9\x82\x82\x82\x82", 5},
  {"\xfd\x82\x82\x82\x82\x82", 6},
};

INSTANTIATE_TEST_SUITE_P(nr_utf8bytes_tests,
                        nr_utf8bytes_test,
                        testing::ValuesIn(nr_utf8bytes_testcases));

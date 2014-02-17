#include "char_stream.hh"
#include <sstream>
#include <gtest/gtest.h>

using namespace std;
using namespace kyaml;

TEST(char_stream_test, simple_get)
{
  stringstream str("a");
  char_stream cs(str);

  char_t c;
  EXPECT_TRUE(cs.get(c));
  EXPECT_EQ('a', c);
}

TEST(char_stream_test, simple_peek)
{
  stringstream str("a");
  char_stream cs(str);

  char_t c;
  EXPECT_TRUE(cs.peek(c));
  EXPECT_EQ('a', c);
}

TEST(char_stream_test, multi_get)
{
  string seq = "abc";
  stringstream str(seq);
  char_stream cs(str);

  for(char c : seq)
  {
    char_t a;
    EXPECT_TRUE(cs.get(a));
    EXPECT_EQ(c, a);
  }
}

TEST(char_stream_test, multi_peek)
{
  string seq = "abc";
  stringstream str(seq);
  char_stream cs(str);

  for(size_t i = 0; i < 3; ++i)
  {
    char_t c;
    cs.peek(c);
    EXPECT_EQ(seq[0], c);
  }
}

TEST(char_stream_test, endoffile)
{
  stringstream str("");
  char_t c;
  EXPECT_FALSE(char_stream(str).get(c));
}

TEST(char_stream_test, unwind)
{
  string seq = "abc";
  stringstream str(seq);
  char_stream cs(str);

  char_stream::mark_t m = cs.mark();

  char_t c;
  cs.get(c);
  EXPECT_EQ(seq[0], c);
  cs.unwind(m);
  cs.get(c);
  EXPECT_EQ(seq[0], c);
}

TEST(char_stream_test, unwind_later)
{
  string seq = "abc";
  stringstream str(seq);
  char_stream cs(str);

  char_t c;
  cs.get(c);
  EXPECT_EQ(seq[0], c);
  char_stream::mark_t m = cs.mark();
  cs.get(c);
  EXPECT_EQ(seq[1], c);
  cs.unwind(m);
  cs.get(c);
  EXPECT_EQ(seq[1], c);
}

TEST(char_stream_test, consume)
{
  string seq = "abc";
  stringstream str(seq);
  char_stream cs(str);

  char_t c;
  cs.get(c), cs.get(c);
  EXPECT_EQ(seq.substr(0, 2), cs.consume());
  cs.get(c);
  EXPECT_EQ(seq[2], c);
}

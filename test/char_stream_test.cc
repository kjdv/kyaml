#include "char_stream.hh"
#include <sstream>
#include <gtest/gtest.h>

using namespace std;
using namespace kyaml;

TEST(char_stream_test, simple_get)
{
  stringstream str("a");
  EXPECT_EQ('a', char_stream(str).get());
}

TEST(char_stream_test, simple_peek)
{
  stringstream str("a");
  EXPECT_EQ('a', char_stream(str).peek());
}

TEST(char_stream_test, multi_get)
{
  string seq = "abc";
  stringstream str(seq);
  char_stream cs(str);

  for(char c : seq)
    EXPECT_EQ(c, cs.get());
}

TEST(char_stream_test, multi_peek)
{
  string seq = "abc";
  stringstream str(seq);
  char_stream cs(str);

  for(size_t i = 0; i < 3; ++i)
    EXPECT_EQ(seq[0], cs.peek());
}

TEST(char_stream_test, endoffile)
{
  stringstream str("");
  EXPECT_LT(char_stream(str).get(), 0);
}

TEST(char_stream_test, unwind)
{
  string seq = "abc";
  stringstream str(seq);
  char_stream cs(str);

  char_stream::mark_t m = cs.mark();

  EXPECT_EQ(seq[0], cs.get());
  cs.unwind(m);
  EXPECT_EQ(seq[0], cs.get());
}

TEST(char_stream_test, consume)
{
  string seq = "abc";
  stringstream str(seq);
  char_stream cs(str);

  cs.get(), cs.get();
  EXPECT_EQ(seq.substr(0, 2), cs.consume());
  EXPECT_EQ(seq[2], cs.get());
}

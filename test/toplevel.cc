#include "kyaml.hh"
#include <cassert>
#include <gtest/gtest.h>

using namespace std;
using namespace kyaml;

namespace
{
  shared_ptr<document> parse(string const &str)
  {
    stringstream stream(str);
    return kyaml::parse(stream);
  }

  string const &value(node const &n)
  {
    return n.get();
  }

  template <typename... tail_t>
  string const &value(node const &n, size_t idx, tail_t... tail)
  {
    return value(n.get(idx), tail...);
  }

  template <typename... tail_t>
  string const &value(node const &n, std::string const &key, tail_t... tail)
  {
    return value(n.get(key), tail...);
  }
}

TEST(toplevel, simple_sequence)
{
  string input = "[ one, two ]";
  shared_ptr<document> doc = parse(input);

  EXPECT_TRUE((bool)doc);

  EXPECT_EQ("one", value(*doc, 0));
  EXPECT_EQ("two", value(*doc, 1));
}

TEST(toplevel, simple_mapping)
{
  string input = "{ key : value }";
  shared_ptr<document> doc = parse(input);

  EXPECT_TRUE((bool)doc);

  EXPECT_EQ("value", value(*doc, "key"));
}

TEST(toplevel, DISABLED_nested)
{
  string input = "[ [one, two] ]";
  shared_ptr<document> doc = parse(input);

  EXPECT_TRUE((bool)doc);

  EXPECT_EQ("one", value(*doc, 0, 0));
  EXPECT_EQ("two", value(*doc, 0, 1));
}

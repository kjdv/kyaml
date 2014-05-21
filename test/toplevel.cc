#include "kyaml.hh"
#include <cassert>
#include <gtest/gtest.h>

using namespace std;
using namespace kyaml;

class toplevel : public testing::Test
{
public:

  void parse(std::string const &input)
  {
    stringstream stream(input);
    d_document = kyaml::parse(stream);

    ASSERT_TRUE((bool)d_document);
  }

  template <typename... path_t>
  string const &value(path_t... path) const
  {
    assert(d_document);
    return value_r(*d_document, path...);
  }

private:
  string const &value_r(node const &n) const
  {
    return n.get();
  }

  template <typename... tail_t>
  string const &value_r(node const &n, size_t idx, tail_t... tail) const
  {
    return value_r(n.get(idx), tail...);
  }

  template <typename... tail_t>
  string const &value_r(node const &n, std::string const &key, tail_t... tail) const
  {
    return value_r(n.get(key), tail...);
  }

  shared_ptr<document> d_document;
};

TEST_F(toplevel, simple_sequence)
{
  parse("[ one, two ]");

  EXPECT_EQ("one", value(0));
  EXPECT_EQ("two", value(1));
}

TEST_F(toplevel, simple_mapping)
{
  parse("{ key : value }");
  EXPECT_EQ("value", value("key"));
}

TEST_F(toplevel, nested)
{
  parse("[ [one, two] ]");

  EXPECT_EQ("one", value(0, 0));
  EXPECT_EQ("two", value(0, 1));
}

TEST_F(toplevel, flow_sequence_as_value)
{
  parse("{ key : [value1, value2]}");
  EXPECT_EQ("value1", value("key", 0));
  EXPECT_EQ("value2", value("key", 1));
}


TEST_F(toplevel, block_sequence_as_value)
{
  parse("key:\n"
        "  - value1\n"
        "  - value2\n");

  EXPECT_EQ("value1", value("key", 0));
  EXPECT_EQ("value2", value("key", 1));
}

TEST_F(toplevel, DISABLED_mapping_inside_sequence_inside_mapping) // todo: fix this
{
  parse("topkey:\n"
        "  - bottomkey1: value1.1\n"
        "    bottomkey2: value1.2\n"
        "  - bottomkey1: value2.1\n"
        "    bottomkey2: value2.2\n");

  EXPECT_EQ("value1.1", value("topkey", 0, "bottomkey1"));
  EXPECT_EQ("value2.2", value("topkey", 2, "bottomkey2"));
}

TEST_F(toplevel, nontrivial)
{
  parse("receipt:     Oz-Ware Purchase Invoice\n"
        "date:        2012-08-06\n"
        "customer:\n"
        "  given:   Dorothy\n"
        "  family:  Gale\n");

  EXPECT_EQ("Oz-Ware Purchase Invoice", value("receipt"));
  EXPECT_EQ("2012-08-06", value("date"));
  EXPECT_EQ("Dorothy", value("customer", "given"));
  EXPECT_EQ("Gale", value("customer", "family"));
}


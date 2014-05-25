#include "kyaml.hh"
#include "sample_docs.hh"
#include <cassert>
#include <gtest/gtest.h>

using namespace std;
using namespace kyaml;
using namespace kyaml::test;

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
  void check(std::string const &expect, path_t... path)
  {
    ASSERT_TRUE((bool)d_document) << "no valid document";
    ASSERT_TRUE(d_document->has(path...)) << "path " << tostring(path...) << " not present in " << *d_document;
    ASSERT_TRUE(d_document->has_leaf(path...)) << "path " << tostring(path...) << " present, but is not a leaf in " << *d_document;

    EXPECT_EQ(expect, d_document->leaf_value(path...));
  }

private:
  string tostring() const
  {
    return "";
  }

  template <typename head_t, typename... tail_t>
  string tostring(head_t const &head, tail_t... tail) const
  {
    stringstream str;
    str << '.' << head << tostring(tail...);
    return str.str();
  }


  unique_ptr<const document> d_document;
};

TEST_F(toplevel, simple_sequence)
{
  parse("[ one, two ]");

  check("one", 0);
  check("two", 1);
}

TEST_F(toplevel, simple_mapping)
{
  parse("{ key : value }");
  check("value", "key");
}

TEST_F(toplevel, nested)
{
  parse("[ [one, two] ]");

  check("one", 0, 0);
  check("two", 0, 1);
}

TEST_F(toplevel, flow_sequence_as_value)
{
  parse("{ key : [value1, value2]}");
  check("value1", "key", 0);
  check("value2", "key", 1);
}


TEST_F(toplevel, block_sequence_as_value)
{
  parse("key:\n"
        "  - value1\n"
        "  - value2\n");

  check("value1", "key", 0);
  check("value2", "key", 1);
}

TEST_F(toplevel, mapping_inside_sequence_inside_mapping)
{
  parse("topkey:\n"
        "  - bottomkey1: value1.1\n"
        "    bottomkey2: value1.2\n"
        "  - bottomkey1: value2.1\n"
        "    bottomkey2: value2.2\n");

  check("value1.1", "topkey", 0, "bottomkey1");
  check("value2.2", "topkey", 1, "bottomkey2");
}

TEST_F(toplevel, nontrivial)
{
  parse(g_oz_yaml);

  check("Oz-Ware Purchase Invoice", "receipt");
  check("2012-08-06", "date");
  check("Dorothy", "customer", "given");
  check("Gale", "customer", "family");
}

TEST_F(toplevel, anchors)
{
  parse(g_anchors_yaml);

  check("1mm", 0, "step", "spotSize");
  check("1mm", 4, "step", "spotSize");
}

TEST_F(toplevel, newline_preserved)
{
  parse(g_oz_yaml);

  const string expect = "123 Tornado Alley\n"
                        "Suite 16\n";

  check(expect, "bill-to", "street");
}

TEST_F(toplevel, newline_folded)
{
  parse(g_oz_yaml);

  const string expect = "Follow the Yellow Brick "
                        "Road to the Emerald City. "
                        "Pay no attention to the "
                        "man behind the curtain.\n";

  check(expect, "specialDelivery");
}

class datatypes : public toplevel
{
public:
  void SetUp() override
  {
    toplevel::SetUp();
    parse(g_datatypes_yaml);
  }
};

TEST_F(datatypes, integertype)
{
  check("1", "integer");
}

TEST_F(datatypes, stringtype)
{
  check("123", "string");
}

TEST_F(datatypes, floattype)
{
  check("3.14", "float");
}

TEST_F(datatypes, explicit_floattype)
{
  check("123", "explicit_float");
}

TEST_F(datatypes, explicit_stringtype)
{
  check("123", "explicit_string");
}

TEST_F(datatypes, booltype_yes)
{
  check("Yes", "bool_yes");
}

TEST_F(datatypes, booltype_no)
{
  check("No", "bool_no");
}


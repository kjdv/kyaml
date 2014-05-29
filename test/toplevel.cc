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

    kyaml::parser p(stream);
    d_document = p.parse();
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

TEST_F(toplevel, chomp_strip)
{
  const string expect = "The final line\n"
                        "break should be\n"
                        "stripped."; // no trailing \n

  parse(g_chomp_yaml);

  check(expect, "stripped");
}

TEST_F(toplevel, chomp_clip)
{
  const string expect = "The final line\n"
                        "break should be\n"
                        "clipped.\n"; // trailing \n included, no follow-ups

  parse(g_chomp_yaml);

  check(expect, "clipped");
}

TEST_F(toplevel, chomp_keep)
{
  const string expect = "The final line\n"
                        "break should be\n"
                        "kept.\n\n"; // trailing \n and empty newline

  parse(g_chomp_yaml);

  check(expect, "keep");
}

TEST_F(toplevel, chomp_as_space)
{
  const string expect = "This should only have spaces.";
  parse(g_chomp_yaml);

  check(expect, "stripped as space");
}

TEST_F(toplevel, indent_in_string_literal)
{
  const string input = "|\n"
                       "line\n"
                       "  indented\n"
                       "less indented\n";
  const string expect = "line\n  indented\nless indented\n";

  parse(input);

  check(expect);
}

TEST_F(toplevel, indent_in_string_literal_as_space)
{
  const string input = ">\n"
                       "line\n"
                       "\tindented\n"
                       "less indented\n";
  const string expect = "line \tindented less indented\n";

  parse(input);

  check(expect);
}

TEST_F(toplevel, with_directive)
{
  const string input = "%YAML 1.2\n"
                       "---\n"
                       "# with directive\n"
                       "sequence:\n"
                       "  - item 1\n"
                       "  - item 2\n"
                       "\n";

  parse(input);

  check("item 1", "sequence", 0);
  check("item 2", "sequence", 1);
}

TEST_F(toplevel, reserved_directive)
{
  const string input = "%BLAH\n"
                       "---\n"
                       "# with directive\n"
                       "sequence:\n"
                       "  - item 1\n"
                       "  - item 2\n"
                       "\n";

  parse(input);

  check("item 1", "sequence", 0);
  check("item 2", "sequence", 1);
}

TEST_F(toplevel, empty_document)
{
  const string input = "";

  parse(input);
  check("");
}

TEST_F(toplevel, empty_document_prefixed)
{
  const string input =
         "%YAML 1.2\n"
         "---";

  parse(input);
  check("");
}

TEST_F(toplevel, empty_document_suffixed)
{
  const string input =
         "#empty\n"
         "...";

  parse(input);
  check("");
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

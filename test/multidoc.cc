#include "kyaml.hh"
#include "sample_docs.hh"
#include <gtest/gtest.h>

using namespace std;
using namespace kyaml;
using namespace kyaml::test;

TEST(multidoc, single)
{
  stringstream stream(g_multi_yaml);

  kyaml::parser prs(stream);
  unique_ptr<const document> root = prs.parse();

  // document 1 ends with "...", document 2 starts with "%YAML 1.2":
  // ...
  // %YAML 1.2
  EXPECT_EQ("%YAML 1.2\n", prs.peek(10));
}

TEST(multidoc, multi)
{
  stringstream stream(g_multi_yaml);

  try
  {
    kyaml::parser prs(stream);

    // stream 1

    unique_ptr<const document> root = prs.parse();
    ASSERT_TRUE((bool)root);

    EXPECT_EQ("bare document", root->leaf_value());

    // stream 2
    root = prs.parse();
    ASSERT_TRUE((bool)root);

    EXPECT_EQ("item 1", root->leaf_value("sequence", 0));
    EXPECT_EQ("item 2", root->leaf_value("sequence", 1));

    EXPECT_EQ("---\n", prs.peek(4));

    // stream 3
    root = prs.parse();
    ASSERT_TRUE((bool)root);

    EXPECT_EQ("value 1", root->leaf_value("mapping", "key1"));
    EXPECT_EQ("value 2", root->leaf_value("mapping", "key2"));

    // eof
    EXPECT_TRUE(stream.eof());
  }
  catch(parser::parse_error const &e)
  {
    string buf;
    getline(stream, buf, '\0');
    FAIL() << e.what() << "\n\tstream at " << buf;
  }
}

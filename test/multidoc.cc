#include "kyaml.hh"
#include "sample_docs.hh"
#include <gtest/gtest.h>

using namespace std;
using namespace kyaml;
using namespace kyaml::test;

TEST(multidoc, stream)
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

    // stream 3
    root = prs.parse();
    ASSERT_TRUE((bool)root);

    EXPECT_EQ("value 1", root->leaf_value("mapping", "key 1"));
    EXPECT_EQ("value 2", root->leaf_value("mapping", "key 2"));

    // eof
    EXPECT_TRUE(stream.eof());
  }
  catch(parser::parse_error const &e)
  {
    string next;
    getline(stream, next, '\0');

    FAIL() << e.what() << "\n\tstream at " << next;
  }
}

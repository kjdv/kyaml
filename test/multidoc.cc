#include "kyaml.hh"
#include "sample_docs.hh"
#include <gtest/gtest.h>

using namespace std;
using namespace kyaml;
using namespace kyaml::test;

namespace
{
  string remaining(istream &stream)
  {
    string result;
    getline(stream, result, '\0');
    return result;
  }
}

TEST(multidoc, DISABLED_single)
{
  stringstream stream(g_multi_yaml);

  kyaml::parser prs(stream);
  unique_ptr<const document> root = prs.parse();

  // document 1 ends with:
  // ...
  // %YAML
  string head = remaining(stream);
  EXPECT_EQ("%YAML 1.2\n", head.substr(0, 10)) << head;
}

TEST(multidoc, DISABLED_multi)
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

    string head = remaining(stream);
    EXPECT_EQ("---\n", head.substr(0, 4)) << head;

    /*
    // stream 3
    root = prs.parse();
    ASSERT_TRUE((bool)root);

    EXPECT_EQ("value 1", root->leaf_value("mapping", "key 1"));
    EXPECT_EQ("value 2", root->leaf_value("mapping", "key 2"));

    // eof
    EXPECT_TRUE(stream.eof());
    */
  }
  catch(parser::parse_error const &e)
  {
    FAIL() << e.what() << "\n\tstream at " << remaining(stream);
  }
  catch(node::wrong_type const &e)
  {
    FAIL() << e.what() << "\n\tstream at " << remaining(stream);
  }
}

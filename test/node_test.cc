#include "kyaml.hh"
#include "sample_docs.hh"
#include <cassert>
#include <gtest/gtest.h>

using namespace std;
using namespace kyaml;
using namespace kyaml::test;

class node_properties_test : public testing::Test
{
public:
  void SetUp() override
  {
    stringstream stream(g_datatypes_yaml);
    d_document = kyaml::parse(stream);

    ASSERT_TRUE((bool)d_document);
  }

  node const &root() const
  {
    assert(d_document);
    return *d_document;
  }

  template <typename... path_t>
  void check(std::string const &property, path_t... path)
  {
    EXPECT_TRUE(root().has(path...)) << "document " << *d_document << " has no node " << tostring(path...);

    node const &n = root().value(path...);

    EXPECT_TRUE(n.has_property(property));
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

TEST_F(node_properties_test, has)
{
  check("!!float", "explicit_float");
}

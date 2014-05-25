#include "kyaml.hh"
#include "sample_docs.hh"
#include <cassert>
#include <gtest/gtest.h>

using namespace std;
using namespace kyaml;
using namespace kyaml::test;

class node_base_test : public testing::Test
{
protected:
  void parse(std::string const &input)
  {
    stringstream stream(input);
    d_document = kyaml::parse(stream);

    ASSERT_TRUE((bool)d_document);
  }

  node const &root() const
  {
    assert(d_document);
    return *d_document;
  }

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

private:
  unique_ptr<const document> d_document;
};

class node_properties_test : public node_base_test
{
public:
  void SetUp() override
  {
    parse(g_datatypes_yaml);
  }

  template <typename... path_t>
  void check(std::string const &property, path_t... path)
  {
    ASSERT_TRUE(root().has(path...)) << "document " << root() << " has no node " << tostring(path...);

    node const &n = root().value(path...);

    EXPECT_TRUE(n.has_property(property));
  }
};

TEST_F(node_properties_test, has)
{
  check("!!float", "explicit_float");
}

class node_typeconvert_test : public node_base_test
{
public:
  void SetUp() override
  {
    parse(g_datatypes_yaml);
  }

  template <typename expect_t, typename... path_t>
  expect_t get(path_t... path)
  {
    EXPECT_TRUE(root().has_leaf(path...)) << "document " << root() << " has no leaf " << tostring(path...);
    assert(root().has_leaf(path...));

    scalar const &leaf = root().value(path...).as_scalar();

    return leaf.as<expect_t>();
  }

  template <typename expect_t, typename... path_t>
  void check(expect_t const &expect, path_t... path)
  {
    ASSERT_TRUE(root().has_leaf(path...)) << "document " << root() << " has no leaf " << tostring(path...);
    EXPECT_EQ(expect, get<expect_t>(path...));
  }
};

TEST_F(node_typeconvert_test, floattype)
{
  EXPECT_FLOAT_EQ(3.14, get<float>("float"));
}

TEST_F(node_typeconvert_test, floattype_asdouble)
{
  EXPECT_DOUBLE_EQ(3.14, get<double>("float"));
}

TEST_F(node_typeconvert_test, explicit_floattype)
{
  EXPECT_FLOAT_EQ(123, get<float>("explicit_float"));
}

TEST_F(node_typeconvert_test, bool_yes)
{
  check(true, "bool_yes");
}

TEST_F(node_typeconvert_test, bool_no)
{
  check(false, "bool_no");
}

TEST_F(node_typeconvert_test, integertype)
{
  check(1, "integer");
}

TEST_F(node_typeconvert_test, base64)
{
  const string expect = "This is a sample string that will be base64 encoded.";

  vector<uint8_t> binary = get<vector<uint8_t> >("binary");
  string actual(binary.begin(), binary.end());

  EXPECT_EQ(expect, actual);
}

TEST_F(node_typeconvert_test, integer_as_bool)
{
  check(true, "integer");
}

TEST_F(node_typeconvert_test, string_as_bool)
{
  check(false, "simple_string");
}

#include "node_clauses.hh"
#include "clause_test.hh"
#include <gmock/gmock.h>

using namespace std;
using namespace kyaml;
using namespace kyaml::test;
using namespace kyaml::clauses;
using namespace testing;

namespace
{
  class mock_builder : public document_builder
  {
  public:
    // not (yet?) interested in these:
    void add(char const *tag, void_item const &v) override
    {}
    void add(char const *tag, std::string const &v) override
    {}
  
    MOCK_METHOD1(add_anchor, void(string const &));
    MOCK_METHOD1(add_alias, void(string const &));
    MOCK_METHOD1(add_scalar, void(string const &));
  };
}

class properties_test : public Test
{
public:

  bool parse(std::string const &input)
  {
    context_wrap cw(input);
    return properties(cw.get()).parse(builder());
  }

  mock_builder &builder()
  {
    return d_builder;
  }

  void expect_anchor(std::string const &name)
  {
    EXPECT_CALL(builder(), add_anchor(name)).
      Times(1);
  }

  void expect_scalar(std::string const &value)
  {
    EXPECT_CALL(builder(), add_scalar(value)).
      Times(1);
  }
private:
  mock_builder d_builder;
};

TEST_F(properties_test, anchor)
{
  string const name = "anchor";
  string input = string("&") + name + " ";
  
  expect_anchor(name);
  EXPECT_TRUE(parse(input));
}

TEST_F(properties_test, verbatim_tag)
{
  string const value = "kl@2549#[]=0";
  string input = string("!<") + value + ">";
  
  expect_scalar(value);
  EXPECT_TRUE(parse(input));
}

TEST(alias_test, alias)
{
  string const name = "anchor";
  string input = string("*") + name + " ";
  
  context_wrap cw(input);
  alias_node an(cw.get());
  mock_builder mb;
  EXPECT_CALL(mb, add_alias(name)).
    Times(1);
  EXPECT_TRUE(an.parse(mb));
}

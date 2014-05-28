#include "kyaml.hh"
#include "sample_docs.hh"
#include "utils.hh"
#include <gtest/gtest.h>

using namespace std;
using namespace kyaml;
using namespace kyaml::test;

class multidoc_base : public testing::Test
{
public:
  void check_sync(std::string const &head, unsigned linenumber)
  {
    ASSERT_TRUE((bool)d_parser);
    EXPECT_EQ(head, d_parser->peek(head.size()));
    EXPECT_EQ(linenumber, d_parser->linenumber());
  }

  unique_ptr<const document> parse(unsigned n = 0)
  {
    for(unsigned i = 1; i < n; ++i)
      d_parser->parse();

    return d_parser->parse();
  }

  std::stringstream const &stream() const
  {
    return d_stream;
  }

protected:
  void construct(std::string const &input)
  {
    d_stream = stringstream(input);
    d_parser.reset(new kyaml::parser(d_stream));
  }

private:
  stringstream d_stream;
  unique_ptr<kyaml::parser> d_parser;
};

class multidoc : public multidoc_base
{
public:
  void SetUp() override
  {
    multidoc_base::SetUp();
    construct(g_multi_yaml);
  }
};

TEST_F(multidoc, single)
{
  unique_ptr<const document> root = parse();

  // document 1 ends with "...", document 2 starts with "%YAML 1.2":
  // ...
  // %YAML 1.2
  check_sync("%YAML 1.2\n", 6);
}

TEST_F(multidoc, stream)
{
  // stream 1

  unique_ptr<const document> root = parse();
  ASSERT_TRUE((bool)root);

  EXPECT_EQ("bare document", root->leaf_value());

  // stream 2
  check_sync("%YAML 1.2\n", 6);

  root = parse();
  ASSERT_TRUE((bool)root);

  EXPECT_EQ("item 1", root->leaf_value("sequence", 0));
  EXPECT_EQ("item 2", root->leaf_value("sequence", 1));

  // stream 3
  check_sync("---\n", 13);

  root = parse();
  ASSERT_TRUE((bool)root);

  EXPECT_EQ("value 1", root->leaf_value("mapping", "key1"));
  EXPECT_EQ("value 2", root->leaf_value("mapping", "key2"));

  // eof
  EXPECT_TRUE(stream().eof());
}

// the purpose of the unhappy stream test is not as such to lay down the
// one and only rules for what happens on invalid input, but rather
// that (1) something reasonable happends and (2) the stream is synced
// for the next document.
class unhappy : public multidoc_base
{
public:
  void SetUp() override
  {
    multidoc_base::SetUp();
    construct(g_unhappy_stream_yaml);
  }

  unique_ptr<const document> parse(unsigned n = 0)
  {
    if(n > 0)
      skip(n);

    return multidoc_base::parse();
  }

  void error(unsigned n, unsigned linenumber)
  {
    try
    {
      parse(n);

      // if we reach this, no exception was thrown
      FAIL() << "no exception was thrown";
    }
    catch(parser::parse_error const &e)
    {
      logger<false>("parse error")(e.what());
      EXPECT_EQ(linenumber, e.linenumber());
    }
  }

private:
  void skip(unsigned n)
  {
    // by putting the unhappy scenarios in the same stream we implicitely
    // test for stream synchronization on errors
    for(unsigned i = 0; i < n; ++i)
    {
      try
      {
        multidoc_base::parse();
      }
      catch(parser::parse_error const &e)
      {}
    }
  }
};

TEST_F(unhappy, unbalanced_quote)
{
  error(0, 3);
  check_sync("---\n# eos 1", 6);
}

TEST_F(unhappy, empty)
{
  error(1, 12);
  check_sync("---\n# eos 2", 14);
}

TEST_F(unhappy, unbalanced)
{
  error(2, 17);
  check_sync("---\n# eos 3", 19);
}

TEST_F(unhappy, indent)
{
  error(3, 25);
  check_sync("---\n# eos 4", 26);
}


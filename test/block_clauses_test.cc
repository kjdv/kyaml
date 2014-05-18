#include "block_clauses.hh"
#include "clause_test.hh"

using namespace std;
using namespace kyaml;
using namespace kyaml::test;
using namespace kyaml::clauses;

TEST(indentation_detect, explicit_indent)
{
  context_wrap ctx("4");

  indentation_indicator i(ctx.get());
  string_document_builder b;
  i.parse(b);
  EXPECT_EQ(4, ctx.get().indent_level());
  EXPECT_EQ(1, ctx.get().stream().pos());
}

TEST(indentation_detect, explicit_indent_zero)
{
  context_wrap ctx("0");

  indentation_indicator i(ctx.get());
  string_document_builder b;

  EXPECT_FALSE(i.parse(b));
  EXPECT_EQ(0, ctx.get().indent_level());
  EXPECT_EQ(0, ctx.get().stream().pos());
}

TEST(indentation_detect, autodetect_oneline)
{
  context_wrap ctx("  blah");

  indentation_indicator i(ctx.get());
  string_document_builder b;
  i.parse(b);
  EXPECT_EQ(2, ctx.get().indent_level());
  EXPECT_EQ(0, ctx.get().stream().pos());
}

TEST(indentation_detect, autodetect_nextline)
{
  context_wrap ctx("\n   blah");

  indentation_indicator i(ctx.get());
  string_document_builder b;
  i.parse(b);
  EXPECT_EQ(3, ctx.get().indent_level());
  EXPECT_EQ(0, ctx.get().stream().pos());
}

TEST(indentation_detect, autodetect_multiline)
{
  context_wrap ctx("\n \n    blah");

  indentation_indicator i(ctx.get());
  string_document_builder b;
  i.parse(b);
  EXPECT_EQ(4, ctx.get().indent_level());
  EXPECT_EQ(0, ctx.get().stream().pos());
}

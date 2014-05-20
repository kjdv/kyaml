#include "document_clauses.hh"
#include "clause_test.hh"

using namespace std;
using namespace kyaml;
using namespace kyaml::test;
using namespace kyaml::clauses;

TEST(document, bare)
{
  string input = "Bare document";
  context_wrap ctx(input, 0, context::BLOCK_IN);

  bare_document bd(ctx.get());
  null_builder nb;

  EXPECT_TRUE(bd.parse(nb));
  EXPECT_EQ(input.size(), ctx.get().stream().pos());
}

TEST(document, explicit)
{
  string input = "---\nBare document";
  context_wrap ctx(input, 0, context::BLOCK_IN);

  explicit_document ed(ctx.get());
  null_builder nb;

  EXPECT_TRUE(ed.parse(nb));
  EXPECT_EQ(input.size(), ctx.get().stream().pos());
}

TEST(document, directive)
{
  string input = "%YAML 1.2\n---\nBare document\n#comment";
  context_wrap ctx(input);

  directive_document dd(ctx.get());
  null_builder nb;

  EXPECT_TRUE(dd.parse(nb));
  EXPECT_EQ(input.size(), ctx.get().stream().pos());
}

TEST(document, eprefix)
{
  string input = "";

  context_wrap ctx(input);

  document_prefix df(ctx.get());
  null_builder nb;

  EXPECT_TRUE(df.parse(nb));
  EXPECT_EQ(0, ctx.get().stream().pos());
}

TEST(document, prefix)
{
  string input =
      "# comment\n"
      "# lines\n";

  context_wrap ctx(input);

  internal::zero_or_more<document_prefix> df(ctx.get());
  null_builder nb;

  EXPECT_TRUE(df.parse(nb));
  EXPECT_EQ(input.size(), ctx.get().stream().pos());
}

TEST(document, yaml_stream)
{
  string input =
      "# comment\n"
      "# line\n"
      "bare\n"
      "...\n"
      "%YAML 1.2\n"
      "%TAG !! tag:example.com,2000:app/\n"
      "---\n"
      "tagged\n"
      "...";

  context_wrap ctx(input);

  yaml_stream ys(ctx.get());
  null_builder nb;

  EXPECT_TRUE(ys.parse(nb));
  EXPECT_EQ(input.size(), ctx.get().stream().pos());
}

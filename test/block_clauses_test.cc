#include "block_clauses.hh"
#include "clause_test.hh"
#include "mock_builder.hh"

using namespace std;
using namespace kyaml;
using namespace kyaml::test;
using namespace kyaml::clauses;

struct indentation_detect_testcase
{
  const string input;
  const int expect;
  const unsigned consume;
};

class indentation_detect_test : public testing::TestWithParam<indentation_detect_testcase>
{};

TEST_P(indentation_detect_test, detect)
{
  context_wrap ctx(GetParam().input);

  indentation_indicator i(ctx.get());
  null_builder b;

  i.parse(b);

  EXPECT_EQ(GetParam().expect, ctx.get().indent_level());
  EXPECT_EQ(GetParam().consume, ctx.get().stream().pos());
}

indentation_detect_testcase indentation_detect_testcases[] = {
  {"4", 4, 1},
  {"0", -1, 0},
  {"  ", 2, 0},
  {"\n   ", 3, 0},
  {"\n \n    ", 4, 0},
  {"-", 0, 0}
};

INSTANTIATE_TEST_CASE_P(tests_indent_detect_test,
                        indentation_detect_test,
                        testing::ValuesIn(indentation_detect_testcases));

TEST(block_sequence, sequence)
{
  string input =
      "- one\n"
      "- two\n";

  context_wrap ctx(input);

  block_sequence bs(ctx.get());

  mock_builder mb;
  mb.expect_sequence({"one", "two"});

  EXPECT_TRUE(bs.parse(mb));
}

TEST(block_sequence, sequence_single_item)
{
  string input =
      "- one\n";

  context_wrap ctx(input);

  block_sequence bs(ctx.get());

  mock_builder mb;
  mb.expect_sequence({"one"});

  EXPECT_TRUE(bs.parse(mb));
}

TEST(block_sequence, mulitword)
{
  string input =
      "- klaas jacob \n"
      "- de vries\n";

  context_wrap ctx(input);

  block_sequence bs(ctx.get());

  mock_builder mb;
  mb.expect_sequence({"klaas jacob", "de vries"});

  EXPECT_TRUE(bs.parse(mb));
}

TEST(block_sequence, indented)
{
  string input =
      "   - Casablanca\n"
      "   - North by Northwest\n"
      "   - The Man Who Wasn't There\n";

  context_wrap ctx(input, 2);

  block_sequence bs(ctx.get());

  mock_builder mb;
  mb.expect_sequence({"Casablanca", "North by Northwest", "The Man Who Wasn't There"});

  EXPECT_TRUE(bs.parse(mb));
}

TEST(block_mapping, mapping)
{
  string input =
      "key1 : value1\n"
      "key two : value two\n";

  context_wrap ctx(input);

  block_mapping bm(ctx.get());

  mock_builder mb;
  mb.expect_mapping({{"key1", "value1"}, {"key two", "value two"}});

  EXPECT_TRUE(bm.parse(mb));
}

TEST(block_mapping, indented)
{
  string input =
      " name : 'klaas jacob'\n"
      " family name : 'de vries'\n";

  context_wrap ctx(input, 0);

  block_mapping bm(ctx.get());

  mock_builder mb;
  mb.expect_mapping({{"name", "klaas jacob"}, {"family name", "de vries"}});

  EXPECT_TRUE(bm.parse(mb));
}

TEST(compact, sequence)
{
  string input =
      "- one\n"
      "- two\n";

  context_wrap ctx(input, 0);

  compact_sequence cs(ctx.get());

  mock_builder mb;
  mb.expect_sequence({"one", "two"});

  EXPECT_TRUE(cs.parse(mb));
}


TEST(compact, mapping)
{
  string input = "number: 5";

  context_wrap ctx(input);

  compact_mapping cm(ctx.get());

  mock_builder mb;
  mb.expect_mapping({{"number", "5"}});

  EXPECT_TRUE(cm.parse(mb));
}

TEST(line_literal, one_line)
{
  string input = "| \n"
                 "  single line";

  context_wrap ctx(input);

  line_literal ll(ctx.get());

  mock_builder mb;

  EXPECT_CALL(mb, add_scalar("single line")).
    Times(1);

  EXPECT_TRUE(ll.parse(mb));
}

TEST(line_literal, multiline)
{
  string input = "| \n"
                 "line one\n"
                 "line two\n";

  context_wrap ctx(input);

  mock_builder mb;
  line_literal ll(ctx.get());

  EXPECT_CALL(mb, add_scalar("line one\nline two\n")).
    Times(1);

  EXPECT_TRUE(ll.parse(mb));
}

TEST(line_literal, block_header)
{
  string input = "+\nnext";

  context_wrap ctx(input);

  null_builder nb;
  block_header bh(ctx.get());
  EXPECT_TRUE(bh.parse(nb));
  EXPECT_EQ(2, ctx.get().stream().pos());
}

TEST(line_literal, nospace)
{
  string input = "| \n"
                 "line one\n"
                 "line two\n";

  context_wrap ctx(input);

  mock_builder mb;
  line_literal ll(ctx.get());

  EXPECT_CALL(mb, add_scalar("line one\nline two\n")).
    Times(1);

  EXPECT_TRUE(ll.parse(mb));
}

TEST(line_literal, with_indents)
{
  string input = "|\n"
                 "line\n"
                 "  indented\n"
                 "less indented\n";
  context_wrap ctx(input);

  mock_builder mb;
  line_literal ll(ctx.get());

  EXPECT_CALL(mb, add_scalar("line\n  indented\nless indented\n")).
    Times(1);

  EXPECT_TRUE(ll.parse(mb));
}

TEST(folded_content, multiline)
{
  string input = "> \n"
                 "folded content\n"
                 "on two lines\n";

  context_wrap ctx(input);

  mock_builder mb;
  content_folded cf(ctx.get());

  EXPECT_CALL(mb, add_scalar("folded content on two lines\n")).
    Times(1);

  EXPECT_TRUE(cf.parse(mb));
}

#include "block_clauses.hh"
#include "clause_test.hh"

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
  string_document_builder b;

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

  EXPECT_CALL(mb, start_sequence()).
    Times(1);
  EXPECT_CALL(mb, add_scalar("one")).
    Times(1);
  EXPECT_CALL(mb, add_scalar("two")).
    Times(1);
  EXPECT_CALL(mb, end_sequence()).
    Times(1);

  EXPECT_TRUE(bs.parse(mb));
}

TEST(block_sequence, sequence_single_item)
{
  string input =
      "- one\n";

  context_wrap ctx(input);

  block_sequence bs(ctx.get());
  mock_builder mb;

  EXPECT_CALL(mb, start_sequence()).
    Times(1);
  EXPECT_CALL(mb, add_scalar("one")).
    Times(1);
  EXPECT_CALL(mb, end_sequence()).
    Times(1);

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

  EXPECT_CALL(mb, start_sequence()).
    Times(1);
  EXPECT_CALL(mb, add_scalar("klaas jacob")).
    Times(1);
  EXPECT_CALL(mb, add_scalar("de vries")).
    Times(1);
  EXPECT_CALL(mb, end_sequence()).
    Times(1);

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

  EXPECT_CALL(mb, start_sequence()).
    Times(1);
  EXPECT_CALL(mb, add_scalar("Casablanca")).
    Times(1);
  EXPECT_CALL(mb, add_scalar("North by Northwest")).
    Times(1);
  EXPECT_CALL(mb, add_scalar("The Man Who Wasn't There")).
    Times(1);
  EXPECT_CALL(mb, end_sequence()).
    Times(1);

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

  EXPECT_CALL(mb, start_mapping()).
    Times(1);
  EXPECT_CALL(mb, add_scalar("key1")).
    Times(1);
  EXPECT_CALL(mb, add_scalar("value1")).
    Times(1);
  EXPECT_CALL(mb, add_scalar("key two")).
    Times(1);
  EXPECT_CALL(mb, add_scalar("value two")).
    Times(1);
  EXPECT_CALL(mb, end_mapping()).
    Times(1);

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

  EXPECT_CALL(mb, start_mapping()).
    Times(1);
  EXPECT_CALL(mb, add_scalar("name")).
    Times(1);
  EXPECT_CALL(mb, add_scalar("klaas jacob")).
    Times(1);
  EXPECT_CALL(mb, add_scalar("family name")).
    Times(1);
  EXPECT_CALL(mb, add_scalar("de vries")).
    Times(1);
  EXPECT_CALL(mb, end_mapping()).
    Times(1);

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

  EXPECT_CALL(mb, start_sequence()).
    Times(1);
  EXPECT_CALL(mb, add_scalar("one")).
    Times(1);
  EXPECT_CALL(mb, add_scalar("two")).
    Times(1);
  EXPECT_CALL(mb, end_sequence()).
    Times(1);

  EXPECT_TRUE(cs.parse(mb));
}


TEST(compact, mapping)
{
  string input = "number: 5";

  context_wrap ctx(input);

  compact_mapping cm(ctx.get());
  mock_builder mb;

  EXPECT_CALL(mb, start_mapping()).
    Times(1);
  EXPECT_CALL(mb, add_scalar("number")).
    Times(1);
  EXPECT_CALL(mb, add_scalar("5")).
    Times(1);
  EXPECT_CALL(mb, end_mapping()).
    Times(1);

  EXPECT_TRUE(cm.parse(mb));
}

TEST(line_literal, one_line)
{
  string input = "| \n"
                 "  single line\n";

  context_wrap ctx(input, 2);
  ctx.get().stream().advance(3);

  mock_builder mb;

  line_literal_text ll(ctx.get());

  EXPECT_CALL(mb, add_scalar("single line")).
    Times(1);

  EXPECT_TRUE(ll.parse(mb));
}

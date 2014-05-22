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

TEST(block_mapping, DISABLED_sequence_as_value_with_mapping_as_value)
{
  string input = "topkey:\n"
                 "  - bottomkey1: value1.1\n"
                 "    bottomkey2: value1.2\n"
                 "  - bottomkey1: value2.1\n"
                 "    bottomkey2: value2.2\n";

  context_wrap ctx(input, 0, context::BLOCK_OUT);
  ctx.get().stream().advance(7);

  typedef internal::all_of<internal::zero_or_one<internal::and_clause<internal::state_scope<internal::indent_inc_modifier, separate>,
                                                                      internal::state_scope<internal::indent_inc_modifier, properties>
                                                                      >
                                                 >,
                           sline_comment,
                           block_sequence
//                           internal::or_clause<internal::state_scope<seq_spaces, block_sequence>,
//                                               block_mapping>
                           > test_clause;
  null_builder mb;

  test_clause tc(ctx.get());


  EXPECT_TRUE(tc.parse(mb));
 // EXPECT_EQ(input.size(), ctx.get().stream().pos());
}

TEST(block_sequence, DISABLED_mappings_as_value)
{
  string input = "- key1: value1.1\n"
                 "  key2: value1.2\n"
                 "- key1: value2.1\n"
                 "  key2: value2.2\n";

  context_wrap ctx(input, 0);

  typedef block_seq_entry
      //internal::all_of<//internal::simple_char_clause<'-', false>,
      //internal::not_clause<non_white_char>,
      //internal::state_scope<internal::flow_modifier<context::BLOCK_IN>, block_indented> >
  test_clause;
  null_builder mb;

  test_clause tc(ctx.get());

  EXPECT_TRUE(tc.parse(mb));
  //EXPECT_EQ(input.size(), ctx.get().stream().pos());
}

void ns_test_helper(string const &input)
{
  context_wrap ctx(input, 0);
  ctx.get().stream().advance(2);

  typedef internal::state_scope<internal::flow_modifier<context::BLOCK_IN>, block_indented> test_clause;

  null_builder nb;

  test_clause tc(ctx.get());

  EXPECT_TRUE(tc.parse(nb));
  EXPECT_EQ(input.size(), ctx.get().stream().pos());
}

TEST(sequencens, plain)
{
  string input = "- one\n";
  ns_test_helper(input);
}

TEST(sequencens, mapping)
{
  string input = "- key1: value1.1\n"
                 "  key2: value1.2\n";
  ns_test_helper(input);
}

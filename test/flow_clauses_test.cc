#include "flow_clauses.hh"
#include "clause_test.hh"

using namespace std;
using namespace kyaml;
using namespace kyaml::test;
using namespace kyaml::clauses;

// TODO:
// good things once there is a document builder is tests for clauses 137 and 140 as high (mid?) level entry points

TEST(flow_sequence, empty)
{
  context_wrap ctx("[ ]", 0, context::FLOW_IN);

  flow_sequence fs(ctx.get());
  mock_builder b;

  EXPECT_CALL(b, start_sequence()).
    Times(1);
  EXPECT_CALL(b, end_sequence()).
    Times(1);

  EXPECT_TRUE(fs.parse(b));
}

TEST(flow_sequence, single)
{
  context_wrap ctx("[blah]", 0, context::FLOW_IN);

  flow_sequence fs(ctx.get());
  mock_builder b;

  EXPECT_CALL(b, start_sequence()).
    Times(1);
  EXPECT_CALL(b, add_scalar("blah")).
    Times(1);
  EXPECT_CALL(b, end_sequence()).
    Times(1);

  EXPECT_TRUE(fs.parse(b));
}

TEST(flow_sequence, sequence)
{
  context_wrap ctx("[one, two]", 0, context::FLOW_IN);

  flow_sequence fs(ctx.get());
  mock_builder b;

  EXPECT_CALL(b, start_sequence()).
    Times(1);
  EXPECT_CALL(b, end_sequence()).
    Times(1);
  EXPECT_CALL(b, add_scalar("one")).
    Times(1);
  EXPECT_CALL(b, add_scalar("two")).
    Times(1);

  EXPECT_TRUE(fs.parse(b));
}

TEST(flow_sequence, quoted)
{
  context_wrap ctx("[\"one two\", 'three four']", 0, context::FLOW_IN);

  flow_sequence fs(ctx.get());
  mock_builder b;

  EXPECT_CALL(b, start_sequence()).
    Times(1);
  EXPECT_CALL(b, end_sequence()).
    Times(1);
  EXPECT_CALL(b, add_scalar("one two")).
    Times(1);
  EXPECT_CALL(b, add_scalar("three four")).
    Times(1);

  EXPECT_TRUE(fs.parse(b));
}

TEST(flow_sequence, alias)
{
  context_wrap ctx("[*tag]", 0, context::FLOW_IN);

  flow_sequence fs(ctx.get());
  mock_builder b;

  EXPECT_CALL(b, start_sequence()).
    Times(1);
  EXPECT_CALL(b, add_alias("tag")).
    Times(1);
  EXPECT_CALL(b, end_sequence()).
    Times(1);

  EXPECT_TRUE(fs.parse(b));
}

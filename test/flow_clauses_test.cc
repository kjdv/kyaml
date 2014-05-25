#include "flow_clauses.hh"
#include "clause_test.hh"
#include "mock_builder.hh"

using namespace std;
using namespace kyaml;
using namespace kyaml::test;
using namespace kyaml::clauses;

TEST(flow_sequence, empty)
{
  context_wrap ctx("[ ]", 0, context::FLOW_IN);

  flow_sequence fs(ctx.get());

  mock_builder b;
  b.expect_sequence();

  EXPECT_TRUE(fs.parse(b));
}

TEST(flow_sequence, single)
{
  context_wrap ctx("[blah]", 0, context::FLOW_IN);

  flow_sequence fs(ctx.get());

  mock_builder b;
  b.expect_sequence({"blah"});

  EXPECT_TRUE(fs.parse(b));
}

TEST(flow_sequence, sequence)
{
  context_wrap ctx("[one, two]", 0, context::FLOW_IN);

  flow_sequence fs(ctx.get());

  mock_builder b;
  b.expect_sequence({"one", "two"});

  EXPECT_TRUE(fs.parse(b));
}

TEST(flow_sequence, quoted)
{
  context_wrap ctx("[\"one two\", 'three four']", 0, context::FLOW_IN);

  flow_sequence fs(ctx.get());

  mock_builder b;
  b.expect_sequence({"one two", "three four"});

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

TEST(flow_mapping, empty)
{
  context_wrap ctx("{}", 0, context::FLOW_IN);

  flow_mapping fm(ctx.get());

  mock_builder b;
  b.expect_mapping();

  EXPECT_TRUE(fm.parse(b));
}

TEST(flow_mapping, key_value_pair)
{
  context_wrap ctx("one : two", 0, context::FLOW_KEY);

  flow_map_entry fm(ctx.get());

  mock_builder b;

  EXPECT_CALL(b, add_scalar("one")).
    Times(1);
  EXPECT_CALL(b, add_scalar("two")).
    Times(1);

  EXPECT_TRUE(fm.parse(b));
}

TEST(flow_mapping, key_value_pairs)
{
  context_wrap ctx("one : two , three : four", 0, context::FLOW_KEY);

  flow_map_entries fm(ctx.get());
  mock_builder b;

  EXPECT_CALL(b, add_scalar("one")).
    Times(1);
  EXPECT_CALL(b, add_scalar("two")).
    Times(1);
  EXPECT_CALL(b, add_scalar("three")).
    Times(1);
  EXPECT_CALL(b, add_scalar("four")).
    Times(1);

  EXPECT_TRUE(fm.parse(b));
}


TEST(flow_mapping, single)
{
  context_wrap ctx("{ one : two }", 0, context::FLOW_KEY);

  flow_mapping fm(ctx.get());

  mock_builder b;
  b.expect_mapping({{"one", "two"}});

  EXPECT_TRUE(fm.parse(b));
}


TEST(flow_mapping, mapping)
{
  context_wrap ctx("{ one : two, three : four }", 0, context::FLOW_KEY);

  flow_mapping fm(ctx.get());

  mock_builder b;
  b.expect_mapping({{"one", "two"}, {"three", "four"}});

  EXPECT_TRUE(fm.parse(b));
}

TEST(flow_mapping, quoted)
{
  context_wrap ctx("{ 'quoted key' : \"some : quoted , text\" }", 0, context::FLOW_KEY);

  flow_mapping fm(ctx.get());

  mock_builder b;
  b.expect_mapping({{"quoted key", "some : quoted , text"}});

  EXPECT_TRUE(fm.parse(b));
}

TEST(flow_mapping, empty_key)
{
  context_wrap ctx("{ : value }", 0, context::FLOW_KEY);

  flow_mapping fm(ctx.get());

  mock_builder b;
  b.expect_mapping({{"", "value"}});

  EXPECT_TRUE(fm.parse(b));
}

TEST(flow_mapping, empty_value)
{
  context_wrap ctx("{ key : }", 0, context::FLOW_KEY);

  flow_mapping fm(ctx.get());

  mock_builder b;
  b.expect_mapping({{"key", ""}});

  EXPECT_TRUE(fm.parse(b));
}


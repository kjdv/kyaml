#include "block_clauses.hh"
#include "clause_test.hh"

using namespace std;
using namespace kyaml;
using namespace kyaml::test;
using namespace kyaml::clauses;

struct indentation_detect_testcase
{
  const string input;
  const unsigned expect;
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
  {"0", 0, 0},
  {"  ", 2, 0},
  {"\n   ", 3, 0},
  {"\n \n    ", 4, 0}
};

INSTANTIATE_TEST_CASE_P(tests_indentent_detect_test,
                        indentation_detect_test,
                        testing::ValuesIn(indentation_detect_testcases));

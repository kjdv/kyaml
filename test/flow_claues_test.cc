#include "flow_clauses.hh"
#include "clause_test.hh"
#include <gmock/gmock.h>

using namespace std;
using namespace kyaml;
using namespace kyaml::test;
using namespace kyaml::clauses;

class mock_builder : public document_builder
{
public:
  MOCK_METHOD0(start_sequence, void());
  MOCK_METHOD0(end_sequence, void());
  MOCK_METHOD0(start_mapping, void());
  MOCK_METHOD0(end_mapping, void());

  MOCK_METHOD1(add_scalar, void(string const &));
};
    

// TODO:
// good things once there is a document builder is tests for clauses 137 and 140 as high (mid?) level entry points

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

#include "mock_builder.hh"

using namespace std;
using namespace kyaml;
using namespace kyaml::test;

using namespace testing;

void mock_builder::expect_sequence(initializer_list<string> const &seq)
{
  InSequence in_seq;

  EXPECT_CALL(*this, start_sequence()).
      Times(1);

  for(auto const &item : seq)
  {
    EXPECT_CALL(*this, add_scalar(item)).
        Times(1);
  }

  EXPECT_CALL(*this, end_sequence()).
      Times(1);
}

void mock_builder::expect_mapping(const std::initializer_list<std::pair<string, string> > &map)
{
  InSequence in_seq;

  EXPECT_CALL(*this, start_mapping()).
      Times(1);

  for(auto const &kvpair : map)
  {
    EXPECT_CALL(*this, add_scalar(kvpair.first)).
        Times(1);
    EXPECT_CALL(*this, add_scalar(kvpair.second)).
        Times(1);
  }

  EXPECT_CALL(*this, end_mapping()).
      Times(1);
}

#include "tokenizer.hh"
#include <sstream>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace std;
using namespace kyaml;

struct test_token
{
  bool               expect;
  tokenizer::token_t token;
  std::string        input;
};

class single_token : public testing::TestWithParam<test_token>
{};

TEST_P(single_token, token)
{
  bool const expect = GetParam().expect;
  tokenizer::token_t const token = GetParam().token;
  stringstream input(GetParam().input);

  tokenizer t(input);

  EXPECT_EQ(expect, t.next(token)) 
    << '"' << GetParam().input << "\" " << (expect ? "did" : "did not") << "produce token " << token;
}

test_token single_token_testcases[] = 
{
  {true, tokenizer::NONE, ""},
  {true, tokenizer::STREAM_START, ""},
};

INSTANTIATE_TEST_CASE_P(single_token_test,
                        single_token,
                        testing::ValuesIn(single_token_testcases));

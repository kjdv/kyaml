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
  input.seekg(0);

  tokenizer t(input);

  EXPECT_EQ(expect, t.next(token)) 
    << '"' << GetParam().input << "\" " << (expect ? "did not" : "did") << " produce token " << token;
}

test_token single_token_testcases[] = 
{
  {true, tokenizer::NONE, ""},
  {true, tokenizer::STREAM_START, ""},
  {false, tokenizer::STREAM_END, ""},
  {true, tokenizer::DOCUMENT_START, "---"},
  {true, tokenizer::DOCUMENT_START, "--- "},
  {false, tokenizer::DOCUMENT_START, "---a"},
  {false, tokenizer::DOCUMENT_START, "--"},
  {true, tokenizer::DOCUMENT_END, "..."},
  {true, tokenizer::DIRECTIVE, "%"},
  {true, tokenizer::ALIAS, "*key"},
  {false, tokenizer::ALIAS, "*"},
  {true, tokenizer::TAG, "!"},
  {true, tokenizer::TAG, "!<verbatim>"},
  {true, tokenizer::ANCHOR, "&key"},
  {false, tokenizer::ANCHOR, "&"},
  {true, tokenizer::FLOW_SEQUENCE_START, "["},
  {true, tokenizer::FLOW_SEQUENCE_END, "]"},
  {true, tokenizer::FLOW_MAPPING_START, "{"},
  {true, tokenizer::FLOW_MAPPING_END, "}"},
  {true, tokenizer::FLOW_ENTRY, ","},
  {true, tokenizer::BLOCK_ENTRY, "-"},
};

INSTANTIATE_TEST_CASE_P(single_token_test,
                        single_token,
                        testing::ValuesIn(single_token_testcases));

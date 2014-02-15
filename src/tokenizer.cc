#include "tokenizer.hh"
#include <cassert>

using namespace std;
using namespace kyaml;

const tokenizer::next_f_t tokenizer::s_dispatch[] = 
{
  next_f_t(&tokenizer::try_none),
  next_f_t(&tokenizer::try_stream_start),
  next_f_t(&tokenizer::try_stream_end),
  next_f_t(&tokenizer::try_document_start),
  next_f_t(&tokenizer::try_document_end),
  next_f_t(&tokenizer::try_directive),
  next_f_t(&tokenizer::try_alias),
  next_f_t(&tokenizer::try_tag),
  next_f_t(&tokenizer::try_anchor),
  next_f_t(&tokenizer::try_scalar),
  next_f_t(&tokenizer::try_block_sequence_start),
  next_f_t(&tokenizer::try_mapping_squence_start),
  next_f_t(&tokenizer::try_block_entry),
  next_f_t(&tokenizer::try_block_end),
  next_f_t(&tokenizer::try_flow_sequence_start),
  next_f_t(&tokenizer::try_flow_mapping_start),
  next_f_t(&tokenizer::try_flow_entry),
  next_f_t(&tokenizer::try_flow_sequence_end),
  next_f_t(&tokenizer::try_flow_mapping_end),
};

// the above hardcodes enum values, place a static assert for that
static_assert(tokenizer::NONE                 ==  0, "");
static_assert(tokenizer::STREAM_START         ==  1, "");
static_assert(tokenizer::STREAM_END           ==  2, "");
static_assert(tokenizer::DOCUMENT_START       ==  3, "");
static_assert(tokenizer::DOCUMENT_END         ==  4, "");
static_assert(tokenizer::DIRECTIVE            ==  5, "");
static_assert(tokenizer::ALIAS                ==  6, "");
static_assert(tokenizer::TAG                  ==  7, "");
static_assert(tokenizer::ANCHOR               ==  8, "");
static_assert(tokenizer::SCALAR               ==  9, "");
static_assert(tokenizer::BLOCK_SEQUENCE_START == 10, "");
static_assert(tokenizer::BLOCK_MAPPING_START  == 11, "");
static_assert(tokenizer::BLOCK_ENTRY          == 12, "");
static_assert(tokenizer::BLOCK_END            == 13, "");
static_assert(tokenizer::FLOW_SEQUENCE_START  == 14, "");
static_assert(tokenizer::FLOW_MAPPING_START   == 15, "");
static_assert(tokenizer::FLOW_ENTRY           == 16, "");
static_assert(tokenizer::FLOW_SEQUENCE_END    == 17, "");
static_assert(tokenizer::FLOW_MAPPING_END     == 18, "");

bool tokenizer::next(token_t expect)
{
  assert(expect < TOKENTABLE_SIZE);

  next_f_t dispatch = s_dispatch[expect];
  return (this->*dispatch)();
}

bool tokenizer::try_document_start()
{
  return false;
}

bool tokenizer::try_document_end()
{
  return false;
}

bool tokenizer::try_directive()
{
  return false;
}

bool tokenizer::try_alias()
{
  return false;
}

bool tokenizer::try_tag()
{
  return false;
}

bool tokenizer::try_anchor()
{
  return false;
}

bool tokenizer::try_scalar()
{
  return false;
}

bool tokenizer::try_block_sequence_start()
{
  return false;
}

bool tokenizer::try_mapping_squence_start()
{
  return false;
}

bool tokenizer::try_block_entry()
{
  return false;
}

bool tokenizer::try_block_end()
{
  return false;
}

bool tokenizer::try_flow_sequence_start()
{
  return false;
}

bool tokenizer::try_flow_mapping_start()
{
  return false;
}

bool tokenizer::try_flow_entry()
{
  return false;
}

bool tokenizer::try_flow_sequence_end()
{
  return false;
}

bool tokenizer::try_flow_mapping_end()
{
  return false;
}

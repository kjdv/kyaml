#include "tokenizer.hh"
#include "utils.hh"
#include <cassert>

using namespace std;
using namespace kyaml;

namespace 
{
  bool is_char(char c)
  {
    return isprint(c) && !isspace(c);
  }

  bool is_flow_indicator(char c)
  {
    return 
      c == ',' ||
      c == '[' ||
      c == ']' ||
      c == '{' ||
      c == '}';
  }

  bool is_anchor_char(char c)
  {
    return is_char(c) && !is_flow_indicator(c);
  }
}

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
  next_f_t(&tokenizer::try_block_mapping_start),
  next_f_t(&tokenizer::try_block_entry),
  next_f_t(&tokenizer::try_block_end),
  next_f_t(&tokenizer::try_flow_sequence_start),
  next_f_t(&tokenizer::try_flow_mapping_start),
  next_f_t(&tokenizer::try_flow_entry),
  next_f_t(&tokenizer::try_flow_sequence_end),
  next_f_t(&tokenizer::try_flow_mapping_end),
  next_f_t(&tokenizer::try_key),
  next_f_t(&tokenizer::try_value),
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
static_assert(tokenizer::KEY                  == 19, "");
static_assert(tokenizer::VALUE                == 20, "");

bool tokenizer::next(token_t expect)
{
  assert(expect < TOKENTABLE_SIZE);

  next_f_t dispatch = s_dispatch[expect];
  return (this->*dispatch)();
}

bool tokenizer::try_document_start()
{
  string peek = d_stream.peek(4);
  if(startswith_token(peek, "---"))
  {
    d_stream.ignore(peek.size());
    return true;
  }
  return false;
}

bool tokenizer::try_document_end()
{
  string peek = d_stream.peek(4);
  if(startswith_token(peek, "..."))
  {
    d_stream.ignore(peek.size());
    return true;
  }
  return false;
}

bool tokenizer::try_directive()
{
  if(d_stream.peekc() == '%')
  {
    // read until newline
    d_stream.ignore_until('\n');
    return true;
  }
  return false;
}

bool tokenizer::try_alias_anchor(char d)
{
  if(d_stream.peekc() == d)
  {
    d_stream.ignore(1);
    d_value.clear();
    
    int c;
    while((c = d_stream.peekc()) > 0 && is_anchor_char(c))
    {
      d_stream.ignore(1);
      d_value += c;
    }

    if(!d_value.empty())
    {
      return true;
    }
    else
    {
      // restore stream
      d_stream.putback(d);
    }
  }
  return false;
}

bool tokenizer::try_tag()
{
  // TODO: customizable tag by TAG directive
  if(d_stream.peekc() == '!')
  {
    d_stream.ignore(1);
    if(d_stream.peekc() == '<') // verbatim
    {
      d_value.clear();

      int c;
      while((c = d_stream.getc()) >= 0 && c != '>')
        d_value +=c;
        
    }

    return true;
  }
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

bool tokenizer::try_block_mapping_start()
{
  return false;
}

bool tokenizer::try_block_entry()
{
  if(d_stream.peekc() == '-')
  {
    d_stream.ignore(1);
    return true;
  }
  return false;
}

bool tokenizer::try_block_end()
{
  return false;
}

bool tokenizer::try_flow_sequence_start()
{
  if(d_stream.peekc() == '[')
  {
    d_stream.ignore(1);
    return true;
  }
  return false;
}

bool tokenizer::try_flow_mapping_start()
{
  if(d_stream.peekc() == '{')
  {
    d_stream.ignore(1);
    return true;
  }
  return false;
}

bool tokenizer::try_flow_entry()
{
  if(d_stream.peekc() == ',')
  {
    d_stream.ignore(1);
    return true;
  }
  return false;
}

bool tokenizer::try_flow_sequence_end()
{
  if(d_stream.peekc() == ']')
  {
    d_stream.ignore(1);
    return true;
  }
  return false;
}

bool tokenizer::try_flow_mapping_end()
{
  if(d_stream.peekc() == '}')
  {
    d_stream.ignore(1);
    return true;
  }
  return false;
}

bool tokenizer::try_key()
{
  return false;
}

bool tokenizer::try_value()
{
  return false;
}

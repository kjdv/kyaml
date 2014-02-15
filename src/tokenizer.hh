#ifndef TOKENIZER_HH
#define TOKENIZER_HH

#include <algorithm>
#include <functional>
#include <istream>
#include <string>

namespace kyaml
{
  class tokenizer
  {
  public:
    typedef enum 
    {
      NONE,
      STREAM_START,
      STREAM_END,
      DOCUMENT_START,
      DOCUMENT_END,
      DIRECTIVE,
      ALIAS,
      TAG,
      ANCHOR,
      SCALAR,
      BLOCK_SEQUENCE_START,
      BLOCK_MAPPING_START,
      BLOCK_ENTRY,
      BLOCK_END,
      FLOW_SEQUENCE_START,
      FLOW_MAPPING_START,
      FLOW_ENTRY,
      FLOW_SEQUENCE_END,
      FLOW_MAPPING_END,
    } token_t;

    tokenizer(std::istream &stream) : d_stream(stream)
    {}

    // eats the next token iff it is of type expect and return true. If it is of another type,
    // return false. If this returns false this function shall not modify the state of the tokenizer
    bool next(token_t expect);

    std::string const &value() const
    {
      return d_value;
    }

  private:
    bool try_none()
    {
      return true;
    }
    bool try_stream_start()
    {
      return d_stream.good();
    }
    bool try_stream_end()
    {
      return d_stream.eof();
    }
    bool try_document_start();
    bool try_document_end();
    bool try_directive();
    bool try_alias();
    bool try_tag();
    bool try_anchor();
    bool try_scalar();
    bool try_block_sequence_start();
    bool try_mapping_squence_start();
    bool try_block_entry();
    bool try_block_end();
    bool try_flow_sequence_start();
    bool try_flow_mapping_start();
    bool try_flow_entry();
    bool try_flow_sequence_end();
    bool try_flow_mapping_end();

    std::istream &d_stream;
    std::string d_value;

    // static dispact table for bool next()
    typedef bool (tokenizer::*next_f_t)(); // pointer to member function
    enum {TOKENTABLE_SIZE = FLOW_MAPPING_END + 1};
    static const next_f_t s_dispatch[TOKENTABLE_SIZE];
  };
}

#endif // TOKENIZER_HH

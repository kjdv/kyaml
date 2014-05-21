#include "document_builder.hh"

using namespace std;
using namespace kyaml;

void replay_builder::start_sequence()
{
  d_items.emplace_back(START_SEQUENCE);
}

void replay_builder::end_sequence()
{
  d_items.emplace_back(END_SEQUENCE);
}

void replay_builder::start_mapping()
{
  d_items.emplace_back(START_MAPPING);
}

void replay_builder::end_mapping()
{
  d_items.emplace_back(END_MAPPING);
}

void replay_builder::add_anchor(const string &anchor)
{
  d_items.emplace_back(ANCHOR, anchor);
}

void replay_builder::add_alias(const string &alias)
{
  d_items.emplace_back(ALIAS, alias);
}

void replay_builder::add_scalar(const string &val)
{
  d_items.emplace_back(SCALAR, val);
}

void replay_builder::add_atom(char32_t c)
{
  d_items.emplace_back(ATOM, c);
}

void replay_builder::build(document_builder &builder) const
{
  for(auto const &it : d_items)
  {
    switch(it.token)
    {
    case START_SEQUENCE:
      builder.start_sequence();
      break;
    case END_SEQUENCE:
      builder.end_sequence();
      break;
    case START_MAPPING:
      builder.start_mapping();
      break;
    case END_MAPPING:
      builder.end_mapping();
      break;
    case ANCHOR:
      builder.add_anchor(it.value);
      break;
    case ALIAS:
      builder.add_alias(it.value);
      break;
    case SCALAR:
      builder.add_scalar(it.value);
      break;
    case ATOM:
      builder.add_atom(it.atom);
      break;
    default:
      assert(false);
    }
  }
}




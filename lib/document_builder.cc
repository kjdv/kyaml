#include "document_builder.hh"
#include "context.hh"

using namespace std;
using namespace kyaml;

document_builder::context::context(kyaml::context const &ctx) :
  d_linenumber(ctx.linenumber())
{}

void replay_builder::start_sequence(context const &ctx)
{
  d_items.emplace_back(START_SEQUENCE, ctx);
}

void replay_builder::end_sequence(context const &ctx)
{
  d_items.emplace_back(END_SEQUENCE, ctx);
}

void replay_builder::start_mapping(context const &ctx)
{
  d_items.emplace_back(START_MAPPING, ctx);
}

void replay_builder::end_mapping(context const &ctx)
{
  d_items.emplace_back(END_MAPPING, ctx);
}

void replay_builder::add_anchor(context const &ctx, const string &anchor)
{
  d_items.emplace_back(ANCHOR, ctx, anchor);
}

void replay_builder::add_alias(context const &ctx, const string &alias)
{
  d_items.emplace_back(ALIAS, ctx, alias);
}

void replay_builder::add_scalar(context const &ctx, const string &val)
{
  d_items.emplace_back(SCALAR, ctx, val);
}

void replay_builder::add_atom(context const &ctx, char32_t c)
{
  d_items.emplace_back(ATOM, ctx, c);
}

void replay_builder::add_property(context const &ctx, const string &prop)
{
  d_items.emplace_back(PROPERTY, ctx, prop);
}

void replay_builder::replay(document_builder &builder) const
{
  for(auto&& it : d_items)
  {
    switch(it.token)
    {
    case START_SEQUENCE:
      builder.start_sequence(it.ctx);
      break;
    case END_SEQUENCE:
      builder.end_sequence(it.ctx);
      break;
    case START_MAPPING:
      builder.start_mapping(it.ctx);
      break;
    case END_MAPPING:
      builder.end_mapping(it.ctx);
      break;
    case ANCHOR:
      builder.add_anchor(it.ctx, it.value);
      break;
    case ALIAS:
      builder.add_alias(it.ctx, it.value);
      break;
    case SCALAR:
      builder.add_scalar(it.ctx, it.value);
      break;
    case ATOM:
      builder.add_atom(it.ctx, it.atom);
      break;
    case PROPERTY:
      builder.add_property(it.ctx, it.value);
      break;
    default:
      assert(false);
    }
  }
}

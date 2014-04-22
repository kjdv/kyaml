#include "node_clauses.hh"

using namespace std;
using namespace kyaml;
using namespace kyaml::clauses;

bool anchor_char::parse(document_builder &builder)
{
  dummy_document_builder dummy;
  flow_indicator fl(ctx());
  if(fl.parse(dummy))
  {
    unwind();
    return false;
  }

  non_white_char nw(ctx());
  return nw.parse(builder);
}

bool anchor_property::parse(document_builder &builder)
{
  dummy_document_builder dm;
  if(internal::simple_char_clause<'&'>(ctx()).parse(dm))
  {
    string_builder sb;
    anchor_name an(ctx());
    if(an.parse(sb))
    {
      builder.add_anchor(sb.build());
      return true;
    }
    else
      unwind();
  }
  return false;
}

bool verbatim_tag::parse(document_builder &builder)
{
  dummy_document_builder db;
  string_builder sb;

  if(internal::simple_char_clause<'!'>(ctx()).parse(db) &&
     internal::simple_char_clause<'<'>(ctx()).parse(db) &&
     internal::one_or_more<uri_char>(ctx()).parse(sb) &&
     internal::simple_char_clause<'>'>(ctx()).parse(db))
  {
    builder.add_scalar(sb.build());
    return true;
  }
  else
    unwind();
  return false;
}

bool alias_node::parse(document_builder &builder)
{
  dummy_document_builder db;
  string_builder sb;
  if(internal::simple_char_clause<'*'>(ctx()).parse(db) &&
     anchor_name(ctx()).parse(sb))
  {
    builder.add_alias(sb.build());
    return true;
  }
  else
    unwind();
  return false;
}

double_text::double_text(context &ctx) :
  clause(ctx),
  d_dispatch(nullptr)
{
  context::blockflow_t bf = ctx.blockflow();

  switch(bf)
  {
  case context::FLOW_OUT:
  case context::FLOW_IN:
    d_dispatch = &double_text::parse_multiline;
    break;
  case context::BLOCK_KEY:
  case context::FLOW_KEY:
    d_dispatch = &double_text::parse_oneline;
    break;
  default:
    break;
  }
}

bool double_text::parse_multiline(document_builder &builder)
{
  double_multi_line dml(ctx());
  return dml.parse(builder);
}

bool double_text::parse_oneline(document_builder &builder)
{
  double_one_line dol(ctx());
  return dol.parse(builder);
}

single_text::single_text(context &ctx) :
  clause(ctx),
  d_dispatch(nullptr)
{
  context::blockflow_t bf = ctx.blockflow();

  switch(bf)
  {
  case context::FLOW_OUT:
  case context::FLOW_IN:
    d_dispatch = &single_text::parse_multiline;
    break;
  case context::BLOCK_KEY:
  case context::FLOW_KEY:
    d_dispatch = &single_text::parse_oneline;
    break;
  default:
    break;
  }
}

bool single_text::parse_multiline(document_builder &builder)
{
  single_multi_line sml(ctx());
  return sml.parse(builder);
}

bool single_text::parse_oneline(document_builder &builder)
{
  single_one_line sol(ctx());
  return sol.parse(builder);
}

plain_safe::plain_safe(context &ctx) :
  clause(ctx),
  d_dispatch(nullptr)
{
  context::blockflow_t bf = ctx.blockflow();

  switch(bf)
  {
  case context::FLOW_OUT:
  case context::BLOCK_KEY:
    d_dispatch = &plain_safe::parse_safe_out;
    break;

  case context::FLOW_IN:
  case context::FLOW_KEY:
    d_dispatch = &plain_safe::parse_safe_in;
    break;
  default:
    break;
  }
}

bool plain_safe::parse_safe_in(document_builder &builder)
{
  plain_safe_in psi(ctx());
  return psi.parse(builder);
}

bool plain_safe::parse_safe_out(document_builder &builder)
{
  plain_safe_out pso(ctx());
  return pso.parse(builder);
}



bool plain_first::parse(document_builder &builder)
{
  left_t l(ctx());
  if(l.parse(builder))
    return true;

  dummy_document_builder dm;
  augmented_right_t r(ctx());
  if(r.parse(dm))
  {
    unwind();
    right_t real(ctx());
    real.parse(builder);
    return true;
  }
  return false;
}


bool plain_char::parse(document_builder &builder)
{
  clause1_t c1(ctx());
  if(c1.parse(builder))
    return true;
  if(preceded_by_nschar(ctx()))
  {
    clause2_t c2(ctx());
    if(c2.parse(builder))
      return true;
  }
  if(followed_by_plain_safe(ctx()))
  {
    clause3_t c3(ctx());
    if(c3.parse(builder))
      return true;
  }
  return false;
}

bool plain_char::preceded_by_nschar(context &ctx)
{
  char32_t c;
  if(ctx.stream().rpeek(c))
  {
    string s;
    append_utf8(s, c);
    std::stringstream str(s);

    char_stream cs(str);
    context c2(cs);

    non_white_char nwc(c2);
    dummy_document_builder db;
    return nwc.parse(db);
  }
  return false;
}

bool plain_char::followed_by_plain_safe(context &ctx)
{
  char32_t c;
  if(ctx.stream().peek(c))
  {
    string s;
    append_utf8(s, c);
    std::stringstream str(s);

    char_stream cs(str);
    context c2(cs);

    plain_safe ps(c2);
    dummy_document_builder db;
    return ps.parse(db);
  }
  return false;
}

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

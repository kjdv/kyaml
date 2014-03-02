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
  dummy_document_builder db;
  if(internal::simple_char_clause<'&'>(ctx()).parse(db))
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

#include "block_clauses.hh"
#include "document_builder.hh"
#include <sstream>

using namespace std;
using namespace kyaml;
using namespace kyaml::clauses;

namespace
{
  class indent_builder : public document_builder
  {
  public:
    indent_builder() : d_value(0)
    {}

    void add(char const *tag, std::string const &v) override
    {
      stringstream str(v);
      str >> d_value;
    }
    
    unsigned build()
    {
      return d_value;
    }

  private:
    unsigned d_value;
  };
}

bool indentation_indicator::parse(document_builder &builder)
{
  indent_builder ib;

  internal::simple_char_clause<'0', false> z(ctx());
  dec_digit_char d(ctx());
  
  if(z.parse(ib))
  {
    unwind();
    return false;
  }
  else if(d.parse(ib))
  {
    ctx().set_indent(ib.build());
    return true;
  }
  return autodetect(ib);
}

bool indentation_indicator::autodetect(document_builder &builder)
{
  line_break lb(ctx());
  space s(ctx());

  unsigned count;
  do
  {
    count = 0;
    while(s.parse(builder))
      ++count;

  } while(lb.parse(builder));

  unwind();
  ctx().set_indent(count);          
  return true;
}

bool chomping_indicator::parse(document_builder &builder)
{
  internal::simple_char_clause<'-', false> strip(ctx());
  if(strip.parse(builder))
    ctx().set_chomp(context::STRIP);
  else
  {
    internal::simple_char_clause<'+', false> keep(ctx());
    if(keep.parse(builder))
      ctx().set_chomp(context::KEEP);
    else
      ctx().set_chomp(context::CLIP);
  }
  return true;
}

bool chomped_last::parse(document_builder &builder)
{
  if(ctx().chomp() == context::STRIP)
  {
    internal::or_clause<non_content, internal::endoffile> d(ctx());
    return d.parse(builder);
  }
  else
  {
    internal::or_clause<as_line_feed, internal::endoffile> d(ctx());
    return d.parse(builder);
  }
}

bool chomped_empty::parse(document_builder &builder)
{
  if(ctx().chomp() == context::KEEP)
  {
    keep_empty d(ctx());
    return d.parse(builder);
  }
  else
  {
    strip_empty d(ctx());
    return d.parse(builder);
  }
}

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

  class dummy_builder : public document_builder
  {};
}

unsigned kyaml::clauses::internal::delta_indent(context &ctx)
{
  char_stream::mark_t m = ctx.stream().mark();
  space s(ctx);
  dummy_builder b;

  unsigned count = 0;
  while(s.parse(b))
    ++count;

  ctx.stream().unwind(m); // TODO: cleanup by scope guard
  return count;
}

bool kyaml::clauses::internal::autodetect_indent(context &ctx, unsigned minumum)
{
  char_stream::mark_t m = ctx.stream().mark();

  line_break lb(ctx);
  space s(ctx);
  dummy_builder b;

  unsigned count;
  do
  {
    count = 0;
    while(s.parse(b))
      ++count;

  } while(lb.parse(b));

  ctx.stream().unwind(m); // TODO: cleanup by scope guard

  if(count > minumum)
  {
    ctx.set_indent(count);          
    return true;
  }
  else
    return false;
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
  return autodetect();
}

bool indentation_indicator::autodetect()
{
  return internal::autodetect_indent(ctx(), ctx().indent_level());
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

bool seq_spaces::parse(document_builder &builder)
{
  if(ctx().blockflow() == context::BLOCK_OUT)
  {
    unsigned il = ctx().indent_level();
    assert(il > 0);
    ctx().set_indent(--il);
  }
  return ctx().blockflow() == context::BLOCK_IN;
}

bool block_collection::parse(document_builder &builder)
{
  typedef internal::all_of<internal::zero_or_one<internal::and_clause<internal::state_scope<internal::indent_inc_modifier, separate>,
                                                                      internal::state_scope<internal::indent_inc_modifier, properties>
                                                                      >
                                                 >,
                           sline_comment,
                           internal::or_clause<internal::state_scope<seq_spaces, block_sequence>,
                                               block_mapping>
                           > bc_clause;
  bc_clause bc(ctx());
  return bc.parse(builder);
}

bool compact_mapping::parse(document_builder &builder)
{
  typedef internal::and_clause<block_map_entry,
                               internal::zero_or_more<internal::and_clause<indent_clause_eq,
                                                                           block_map_entry>
                                                      >
                               > cm_clause;
  cm_clause cm(ctx());
  return cm.parse(builder);
}

bool block_indented::parse(document_builder &builder)
{
  // TODO: indent magic
  typedef internal::any_of<internal::and_clause<indent_clause_eq,
                                                internal::or_clause<
                                                  internal::state_scope<internal::indent_inc_modifier, compact_sequence>,
                                                  internal::state_scope<internal::indent_inc_modifier, compact_mapping> 
                                                  >
                                                >,
                           block_node,
                           internal::and_clause<enode, sline_comment>
                           > bi_clause;
  bi_clause bi(ctx());
  return bi.parse(builder);
}

bool block_sequence::parse(document_builder &builder)
{
  unsigned n = ctx().indent_level();
  unsigned m = internal::delta_indent(ctx());

  if(m > 0)
  {
    ctx().set_indent(n + m);
    typedef internal::one_or_more<internal::and_clause<indent_clause_eq, block_seq_entry> > bs_clause;
    
    bs_clause bs(ctx());
    bool result = bs.parse(builder);
    
    // TODO: cleanup by scope guard
    ctx().set_indent(n);
    
    return result; 
  }
  return false;
}

bool block_mapping::parse(document_builder &builder)
{
  unsigned n = ctx().indent_level();
  unsigned m = internal::delta_indent(ctx());

  if(m > 0)
  {
    ctx().set_indent(n + m);
    typedef internal::one_or_more<internal::and_clause<indent_clause_eq, block_map_entry> > bs_clause;
    
    bs_clause bs(ctx());
    bool result = bs.parse(builder);
    
    // TODO: cleanup by scope guard
    ctx().set_indent(n);
    
    return result; 
  }
  return false;
}

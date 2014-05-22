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

    void add_atom(char32_t c) override
    {
      string s;
      append_utf8(s, c);
      stringstream str(s);
      str >> d_value;
    }
    
    int build()
    {
      return d_value;
    }

  private:
    int d_value;
  };

  class dummy_builder : public document_builder
  {};

  logger<true> g_block_log("block");
}

int kyaml::clauses::internal::delta_indent(context &ctx)
{
  char_stream::mark_t m = ctx.stream().mark();
  space s(ctx);
  dummy_builder b;

  int count = 0;
  while(s.parse(b))
    ++count;

  ctx.stream().unwind(m); // TODO: cleanup by scope guard
  return count;
}

bool kyaml::clauses::internal::autodetect_indent(context &ctx, int minumum)
{
  char_stream::mark_t m = ctx.stream().mark();

  line_break lb(ctx);
  space s(ctx);
  dummy_builder b;

  int count;
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
    int il = ctx().indent_level();
    ctx().set_indent(--il);
    return true;
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
  bool result = false;

  typedef internal::and_clause<block_map_entry,
                               internal::zero_or_more<internal::and_clause<indent_clause_eq,
                                                                           block_map_entry>
                                                      >
                               > cm_clause;

  cm_clause cm(ctx());
  replay_builder rb;
  if(cm.parse(rb))
  {
    result = true;
    builder.start_mapping();
    rb.replay(builder);
    builder.end_mapping();
  }

  return result;
}

bool block_indented::parse(document_builder &builder)
{
  return
    parse_funky_indent(builder) ||
    parse_normal(builder);
}

bool block_indented::parse_funky_indent(document_builder &builder)
{
  logger<true> log("funky indent");

  log("head at", ctx().stream().pos());

  const int n = ctx().indent_level();
  const int m = ctx().stream().indent_level(n) - n;
  bool result = false;

  log("n =", n, "m =", m);

  ctx().set_indent(m);
  indent_clause_eq ind(ctx());
  if(ind.parse(builder))
  {
    log("indent_clause_eq passed, at", ctx().stream().pos());

    ctx().set_indent(n + m + 1);
    internal::or_clause<compact_sequence, compact_mapping> sm(ctx());
    if(sm.parse(builder))
    {
      log("compact sequence or mapping passed, at", ctx().stream().pos());
      result = true;
    }
  }


  /*// TODO: indent magic
  typedef internal::any_of<internal::and_clause<internal::state_scope<internal::indent_modifier<2>, indent_clause_eq>,
                                                internal::or_clause<
                                                  internal::state_scope<internal::indent_modifier<n + 2 + 1>, compact_sequence>,
                                                  internal::state_scope<internal::indent_modifier<n + 2 + 1>, compact_mapping>
                                                  >
                                                >,
                           block_node,
                           internal::and_clause<enode, sline_comment>
                           > bi_clause;
  bi_clause bi(ctx());

  */

  // TODO: dtor-based cleanup
  if(!result)
    unwind();

  ctx().set_indent(n);

  return result;
}

bool block_indented::parse_normal(document_builder &builder)
{
  internal::or_clause<block_node,
                      internal::and_clause<enode, sline_comment>
                     > normal(ctx());
  return normal.parse(builder);
}

bool block_sequence::parse(document_builder &builder)
{
  int n = ctx().indent_level();
  int d = internal::delta_indent(ctx());
  int m = d - n;

  if(m > 0)
  {
    ctx().set_indent(n + m);

    g_block_log("seq indent at", n+m);

    replay_builder rb;
    typedef internal::one_or_more<internal::and_clause<indent_clause_eq, block_seq_entry> > bs_clause;

    bs_clause bs(ctx());

    bool result = false;
    if(bs.parse(rb))
    {
      result = true;
      builder.start_sequence();
      rb.replay(builder);
      builder.end_sequence();
    }

    // TODO: cleanup by scope guard
    ctx().set_indent(n);
    
    return result; 
  }
  return false;
}

bool block_mapping::parse(document_builder &builder)
{
  int n = ctx().indent_level();
  int d = internal::delta_indent(ctx());
  int m = d - n;

  if(m > 0)
  {
    ctx().set_indent(n + m);

    g_block_log("block indent at", n+m);

    typedef internal::one_or_more<internal::and_clause<indent_clause_eq, block_map_entry> > bs_clause;
    
    bs_clause bs(ctx());
    bool result = false;

    replay_builder rb;
    if(bs.parse(rb))
    {
      result = true;
      builder.start_mapping();
      rb.replay(builder);
      builder.end_mapping();
    }

    // TODO: cleanup by scope guard
    ctx().set_indent(n);
    
    return result; 
  }
  return false;
}


bool compact_sequence::parse(document_builder &builder)
{
  bool result = false;
  typedef internal::and_clause<block_seq_entry,
                               internal::zero_or_more<internal::and_clause<indent_clause_eq,
                                                                           block_seq_entry> > > cs_clause;

  cs_clause d(ctx());
  replay_builder rb;
  if(d.parse(rb))
  {
    result = true;
    builder.start_sequence();
    rb.replay(builder);
    builder.end_sequence();
  }
  return result;
}


bool block_seq_entry::parse(document_builder &builder)
{
  null_builder nb;
  internal::simple_char_clause<'-', false> minus(ctx());

  g_block_log("se -", stream().pos());
  if(minus.parse(nb))
  {
    internal::not_clause<non_white_char> nw(ctx());
    g_block_log("se nw", stream().pos());
    if(nw.parse(nb))
    {
     // advance(1);
      internal::state_scope<internal::flow_modifier<context::BLOCK_IN>, block_indented> indented(ctx());

      g_block_log("se ind", stream().pos());
      if(indented.parse(builder))
      {
        g_block_log("se done", stream().pos());
        return true;
      }
    }
  }

  unwind();
  return false;
}

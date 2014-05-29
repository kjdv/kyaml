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

    void start_sequence() override
    {}

    void end_sequence() override
    {}

    void start_mapping() override
    {}

    void end_mapping() override
    {}

    void add_anchor(std::string const &) override
    {}

    void add_alias(std::string const &) override
    {}

    void add_scalar(std::string const &) override
    {}

    void add_property(std::string const &) override
    {}
    
    int build()
    {
      return d_value;
    }

  private:
    int d_value;
  };
}

int kyaml::clauses::internal::delta_indent(context &ctx)
{
  stream_guard sg(ctx);

  space s(ctx);
  null_builder b;

  int count = 0;
  while(s.parse(b))
    ++count;

  return count;
}

bool kyaml::clauses::internal::autodetect_indent(context &ctx, int minumum)
{
  context_guard cg(ctx);
  line_break lb(ctx);
  space s(ctx);
  null_builder b;

  int count;
  do
  {
    count = 0;
    while(s.parse(b))
      ++count;

  } while(lb.parse(b));

  if(count > minumum)
  {
    cg.release_state();
    ctx.set_indent(count);          
    return true;
  }
  else
    return false;
}

bool indentation_indicator::parse(document_builder &builder)
{
  logger<false> log("indentation indicator");

  stream_guard sg(ctx());

  log("start", ctx().stream().pos());

  indent_builder ib;

  internal::simple_char_clause<'0', false> z(ctx());
  dec_digit_char d(ctx());
  
  if(z.parse(ib))
  {
    log("0 detected", ctx().stream().pos());
    return false;
  }
  else if(d.parse(ib))
  {
    ctx().set_indent(ib.build());
    log("number detected", ctx().stream().pos());
    sg.release();
    return true;
  }

  bool result = autodetect();
  log("autodetected", result, ctx().stream().pos());
  return result;
}

bool indentation_indicator::autodetect()
{
  return internal::autodetect_indent(ctx(), ctx().indent_level());
}

bool chomping_indicator::parse(document_builder &builder)
{
  logger<false> log("chomping indicator");
  log("start", ctx().stream().pos());

  // as chomping will always be modified before used no state guard is needed (famous last words)

  internal::simple_char_clause<'-', false> strip(ctx());
  if(strip.parse(builder))
  {
    log("strip", ctx().stream().pos());
    ctx().set_chomp(context::STRIP);
  }
  else
  {
    internal::simple_char_clause<'+', false> keep(ctx());
    if(keep.parse(builder))
    {
      log("keep", ctx().stream().pos());
      ctx().set_chomp(context::KEEP);
    }
    else
    {
      log("clip", ctx().stream().pos());
      ctx().set_chomp(context::CLIP);
    }
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
  logger<false> log("funky indent");

  context_guard cg(ctx());

  log("head at", ctx().stream().pos());

  const int n = ctx().indent_level();
  const int m = ctx().stream().indent_level(n) - n;

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
      cg.release_stream();
      return true;
    }
  }
  return false;
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
  state_guard sg(ctx());

  int n = ctx().indent_level();
  int d = internal::delta_indent(ctx());
  int m = d - n;

  if(m > 0)
  {
    ctx().set_indent(n + m);

    replay_builder rb;
    typedef internal::one_or_more<internal::and_clause<indent_clause_eq, block_seq_entry> > bs_clause;

    bs_clause bs(ctx());
    if(bs.parse(rb))
    {
      builder.start_sequence();
      rb.replay(builder);
      builder.end_sequence();

      return true;
    }
  }
  return false;
}

bool block_mapping::parse(document_builder &builder)
{
  state_guard sg(ctx());

  int n = ctx().indent_level();
  int d = internal::delta_indent(ctx());
  int m = d - n;

  if(m > 0)
  {
    ctx().set_indent(n + m);

    typedef internal::one_or_more<internal::and_clause<indent_clause_eq, block_map_entry> > bs_clause;
    
    bs_clause bs(ctx());
    replay_builder rb;
    if(bs.parse(rb))
    {
      builder.start_mapping();
      rb.replay(builder);
      builder.end_mapping();

      return true;
    } 
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


bool literal_content::parse(document_builder &builder)
{
  stream_guard sg(ctx());

  string_builder sb;

  typedef internal::zero_or_one<internal::all_of<line_literal_text,
                                                 internal::zero_or_more<break_literal_next>,
                                                 chomped_last
                                                >
                               > first_t;
  typedef chomped_empty second_t;

  first_t f(ctx());
  if(f.parse(sb))
  {
    second_t s(ctx());
    if(s.parse(sb))
    {
      builder.add_scalar(sb.build());
      sg.release();
      return true;
    }
  }
  return false;
}

bool folded_content::parse(document_builder &builder)
{
  typedef internal::and_clause<internal::zero_or_one<internal::and_clause<diff_lines, chomped_last> >,
                               chomped_empty> delegate_t;

  string_builder sb;

  delegate_t d(ctx());
  if(d.parse(sb))
  {
    builder.add_scalar(sb.build());
    return true;
  }

  return false;
}


bool line_literal::parse(document_builder &builder)
{
  typedef internal::and_clause<internal::simple_char_clause<'|', false>,
                               block_header> head_t;
  typedef literal_content tail_t;

  context_guard cg(ctx());

  null_builder nb;
  head_t head(ctx());

  const int n = ctx().indent_level();

  logger<false> log("line literal");
  log("indent level before", n);

  if(head.parse(nb) &&
     internal::autodetect_indent(ctx(), n) &&
     ctx().indent_level() > 0) // TODO: is indentation at 0 legal here? Anser is no for now, as it will eat until eof
  {


    log("indent level after", ctx().indent_level());
    tail_t tail(ctx());
    if(tail.parse(builder))
    {
      cg.release_stream();
      return true;
    }
  }
  return false;
}


bool content_folded::parse(document_builder &builder)
{
  typedef internal::and_clause<internal::simple_char_clause<'>', false>,
                               block_header> head_t;
  typedef folded_content tail_t;

  context_guard cg(ctx());

  null_builder nb;
  head_t head(ctx());

  const int n = ctx().indent_level();

  if(head.parse(nb) &&
     internal::autodetect_indent(ctx(), n) &&
     ctx().indent_level() > 0) // TODO: is indentation at 0 legal here? Anser is no for now, as it will eat until eof
  {
    tail_t tail(ctx());
    if(tail.parse(builder))
    {
      cg.release_stream();
      return true;
    }
  }
  return false;
}

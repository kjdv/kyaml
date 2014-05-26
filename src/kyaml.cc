#include "kyaml.hh"
#include "clauses.hh"
#include "node_builder.hh"

using namespace std;
using namespace kyaml;
using namespace kyaml::clauses;

namespace
{
  logger<true> g_log("parser");
}

namespace kyaml
{
  bool document_end(context &ctx)
  {
    typedef internal::one_or_more<document_suffix> pos_clause_t;
    typedef internal::and_clause<internal::zero_or_more<document_prefix>, ldirective> pre_clause_t;
    typedef internal::endoffile eof_t;

    ctx.reset();

    null_builder nb;

    eof_t e(ctx);
    if(e.parse(nb))
      return true;
    {
      context_guard cg(ctx);

      pos_clause_t pos(ctx);
      if(pos.parse(nb))
      {
        // eat
        cg.release();
        return true;
      }
    }
    {
      context_guard cg(ctx);

      pre_clause_t pre(ctx);
      if(pre.parse(nb))
        return true; // don't eat
    }

    return false;
  }

  // skipp until the next document in the stream, or eof
  // that is, either until the next ---, past the next ..., or end of file
  void skip_till_next(context &ctx)
  {
    while(!document_end(ctx))
    {
      ctx.stream().ignore('\n');
      ctx.newline();
      ctx.stream().advance(1);
    }
  }

  class skip_guard : private no_copy
  {
  public:
    skip_guard(context &ctx) :
      d_ctx(ctx)
    {}

    ~skip_guard()
    {
      try
      {
        skip_till_next(d_ctx);
      }
      catch(std::exception const &e)
      {
        g_log("exception during cleanup:", e.what());
      }
      catch(...)
      {
        g_log("unspecified exception during cleanup");
      }
    }

  private:
    context &d_ctx;
  };

  class parser_impl
  {
  public:
    parser_impl(istream &input) :
      d_stream(input),
      d_ctx(d_stream, -1, context::NA)
    {}

    unique_ptr<const document> parse()
    {
      skip_guard sg(d_ctx);

      node_builder nb;

      yaml_single_document ys(d_ctx);

      if(ys.parse(nb))
        return nb.build();

      return unique_ptr<const document>();
    }

  private:
    char_stream d_stream;
    context d_ctx;
  };

  parser::parser(istream &input) :
    d_pimpl(new parser_impl(input)) // parser_impl ctor can not throw
  {}

  parser::~parser()
  {}

  std::unique_ptr<const document> parser::parse()
  {
    assert(d_pimpl);
    return d_pimpl->parse();
  }
}

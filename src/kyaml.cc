#include "kyaml.hh"
#include "clauses.hh"
#include "node_builder.hh"

using namespace std;
using namespace kyaml;
using namespace kyaml::clauses;

namespace
{
  logger<false> g_log("parser");
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
      d_ctx.stream().ignore();
      // skip_till_next(d_ctx);
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
      g_log("start parsing at line", d_ctx.linenumber());

      skip_guard sg(d_ctx);

      node_builder nb;

      yaml_single_document ys(d_ctx);

      try // try block temporary, remove when node_builder is fixed, for now this gives better diagnostics
      {
        bool r = ys.parse(nb);
        g_log("done parsing at line", d_ctx.linenumber(), "result", (r ? "true" : "false"));

        if(r)
          return nb.build();
      }
      catch(node_builder::structure_error const &e)
      {
        throw parser::parse_error(d_ctx.linenumber(), e.what());
      }

      throw parser::parse_error(d_ctx.linenumber(), "Could not construct a valid document.");
    }

    string head(size_t n)
    {
      stream_guard sg(d_ctx);

      string result;

      for(size_t i = 0; i < n && d_ctx.stream().good(); ++i)
      {
        char32_t c;
        d_ctx.stream().get(c);
        append_utf8(result, c);
      }

      return result;
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

  string parser::head(size_t n)
  {
    assert(d_pimpl);
    return d_pimpl->head(n);
  }

  parser::parse_error::parse_error(unsigned linenumber, const string &msg) :
    d_linenumber(linenumber)
  {
    stringstream str;
    str << "parse error at line " << d_linenumber;
    if(!msg.empty())
      str << ": " << msg;

    d_msg = str.str();
  }

}

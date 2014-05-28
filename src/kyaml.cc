#include "kyaml.hh"
#include "clauses.hh"
#include "node_builder.hh"

using namespace std;
using namespace kyaml;
using namespace kyaml::clauses;

namespace
{
  logger<false> g_log("parser");

  typedef internal::and_clause<internal::one_or_more<ldirective>,
                               directives_end> start_of_document;
}

namespace kyaml
{
  bool document_end(context &ctx)
  {
    ctx.reset();
    stream_guard sg(ctx);

    forbidden fb(ctx);
    null_builder nb;

    if(fb.parse(nb))
      return true;

    sg.release();
    return false;
  }

  // skipp until the next document in the stream, or eof
  // that is, either until the next ---, past the next ..., or end of file
  void skip_till_next(context &ctx)
  {
    while(ctx.stream().good() && !document_end(ctx))
    {
      //ctx.stream().ignore('\n');
      //ctx.newline();
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
      //d_ctx.stream().ignore();
      skip_till_next(d_ctx);
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

      bool r= ys.parse(nb);

      g_log("done parsing at line", d_ctx.linenumber(), "result", (r ? "true" : "false"));

      if(r)
        return nb.build();

      throw parser::parse_error(d_ctx.linenumber(), "Could not construct a valid document.");
    }

    string peek(size_t n) const
    {
      // trickytrickytricky, peek() is supposed to be semantically const, though discovering the next
      // items in the stream means modifying the stream, so the default 'bitwise-const' won't do.
      // The constness is guaranteed by the context guard, to ensure though the context will be modified
      // during the call it will be restored to its orignal once the call is done.
      // Strictly speaking the context will be modified: the next characters will be read from the buffer
      // instead of the underlying stream, but is behaviourly / semantically unmodified.
      context &ctx = const_cast<parser_impl *>(this)->d_ctx;
      context_guard cg(ctx);

      string result;

      for(size_t i = 0; i < n && ctx.stream().good(); ++i)
      {
        char32_t c;
        ctx.stream().get(c);
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

  string parser::peek(size_t n) const
  {
    assert(d_pimpl);
    return d_pimpl->peek(n);
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

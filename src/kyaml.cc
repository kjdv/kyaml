#include "kyaml.hh"
#include "clauses.hh"
#include "node_builder.hh"

using namespace std;
using namespace kyaml;
using namespace kyaml::clauses;

namespace
{
  logger<true> g_log("parser");

  typedef internal::and_clause<internal::zero_or_more<ldirective>,
                               directives_end> start_of_document;
  typedef internal::or_clause<internal::endoffile,
                              clauses::document_end> end_of_document;

  bool is_document_end(context &ctx)
  {
    context_guard cg(ctx);

    internal::and_clause<internal::zero_or_more<internal::eat_lines>,
                         internal::or_clause<start_of_document,
                                             end_of_document
                                            >
                        > eod(ctx);

    null_builder nb;
    return eod.parse(nb);
  }
}

namespace kyaml
{
  class skip_guard : private no_copy
  {
  public:
    skip_guard(context &ctx) :
      d_ctx(ctx)
    {}

    ~skip_guard()
    {
      d_ctx.stream().ignore();
      skip_till_next();
    }

  private:
    // skipp until the next document in the stream, or eof
    // that is, either until the next ---, past the next ..., or end of file
    void skip_till_next()
    {
      while(d_ctx.stream().good() && !sync_stream())
      {
        // todo: such call might screw up the line number count
        d_ctx.stream().advance(1);
        d_ctx.stream().ignore('\n');
        d_ctx.newline();
      }
    }

    bool sync_stream()
    {
      d_ctx.reset();
      stream_guard sg(d_ctx);

      null_builder nb;
      start_of_document start(d_ctx);

      if(start.parse(nb))
        return true;
      else
      {
        end_of_document end(d_ctx);
        if(end.parse(nb))
        {
          sg.release();
          return true;
        }
      }
      return false;
    }

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

      bool r = ys.parse(nb);

      if(!is_document_end(d_ctx))
        error(string("parsing stopped before the end of document, could not handle \"") + peek(20) + "\"");

      if(r)
        return nb.build();

      error("Could not construct a valid document.");

      return unique_ptr<const document>();
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
    void error(std::string const &msg = "")
    {
      throw parser::parse_error(d_ctx.linenumber(), msg);
    }

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

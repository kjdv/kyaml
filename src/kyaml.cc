#include "kyaml.hh"
#include "clauses.hh"
#include "node_builder.hh"

using namespace std;
using namespace kyaml;
using namespace kyaml::clauses;

namespace kyaml
{
  class parser_impl
  {
  public:
    parser_impl(istream &input) :
      d_stream(input),
      d_ctx(d_stream, -1, context::NA)
    {}

    unique_ptr<const document> parse()
    {
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

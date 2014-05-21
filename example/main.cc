#include <iostream>
#include "document_clauses.hh"
#include "node_builder.hh"
#include "node.hh"

using namespace std;
using namespace kyaml;

namespace
{
  class context_wrap
  {
  public:
    context_wrap(std::string const &s,
                 int indent_level = -1,
                 kyaml::clauses::context::blockflow_t bf = kyaml::clauses::context::NA) :
      d_sstream(s),
      d_stream(d_sstream),
      d_ctx(d_stream, indent_level, bf)
    {}

    kyaml::clauses::context const &get() const
    {
      return d_ctx;
    }

    kyaml::clauses::context &get()
    {
      return d_ctx;
    }
  private:
    std::stringstream d_sstream;
    char_stream d_stream;
    kyaml::clauses::context d_ctx;
  };
}

int main()
{
  string sample = "{ key : value }";

  context_wrap ctx(sample, -1, clauses::context::FLOW_IN);

  clauses::bare_document doc(ctx.get());
  node_builder builder;

  if(doc.parse(builder))
    cout << builder.build();
  else
  {
    cerr << "error\n";
    return 1;
  }

  return 0;
}

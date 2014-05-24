#include "kyaml.hh"
#include "clauses.hh"
#include "node_builder.hh"

using namespace std;
using namespace kyaml;
using namespace kyaml::clauses;

shared_ptr<document> kyaml::parse(istream &input)
{
  char_stream stream(input);
  context ctx(stream, -1, context::NA);

  node_builder nb;

  yaml_single_document ys(ctx);

  if(ys.parse(nb))
    return nb.build();

  return shared_ptr<document>();
}

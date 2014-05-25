#ifndef NODE_VISITOR_HH
#define NODE_VISITOR_HH

#include "node.hh"

namespace kyaml
{
  class node_visitor
  {
  public:
    virtual ~node_visitor()
    {}

    virtual void visit(scalar const &val) = 0;
    virtual void visit(sequence const &seq) = 0;
    virtual void visit(mapping const &map) = 0;
  };
}

#endif // NODE_VISITOR_HH

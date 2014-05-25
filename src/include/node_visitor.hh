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

    // note: a sequence or map will be visited before its containing
    // items. After all items in a sequence or map are visited a call to
    // sentinel() will be made.
    virtual void visit(scalar const &val) = 0;
    virtual void visit(sequence const &seq) = 0;
    virtual void visit(mapping const &map) = 0;

    // optional
    virtual void sentinel(sequence const &seq) // called after a sequence is visited
    {}
    virtual void sentinel(mapping const &map) // called after a mapping is visited
    {}
    virtual void visit_key(size_t idx) // called before visiting an item in a sequence
    {}
    virtual void visit_key(std::string const &key) // called before visiting a value from a mapping
    {}
  };
}

#endif // NODE_VISITOR_HH

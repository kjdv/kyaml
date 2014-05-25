#include <iostream>
#include "kyaml.hh"
#include "node_visitor.hh"

using namespace std;
using namespace kyaml;

namespace
{
  class printer : public node_visitor
  {
  public:
    printer(ostream &out) :
      d_indent(-1),
      d_out(out)
    {}

    void visit(scalar const &val) override
    {
      d_out << val.get() << '\n';
    }

    void visit(sequence const &seq) override
    {
      d_out << '\n';
      ++d_indent;
      for(auto const &item : seq)
      {
        indent();
        d_out << "- ";
        item->accept(*this);
      }
      --d_indent;
    }

    void visit(mapping const &map) override
    {
      d_out << '\n';
      ++d_indent;
      for(auto const &item : map)
      {
        indent();
        d_out << item.first << ": ";
        item.second->accept(*this);
      }
      --d_indent;
    }

  private:
    void indent()
    {
      for(size_t i = 0; i < d_indent; ++i)
        d_out << "  ";
    }

    size_t d_indent;
    ostream &d_out;
  };
}

int main()
{
  shared_ptr<const document> doc = kyaml::parse(std::cin);

  if(doc)
  {
    printer pr(cout);
    doc->accept(pr);
  }
  else
  {
    cerr << "error\n";
    return 1;
  }

  return 0;
}

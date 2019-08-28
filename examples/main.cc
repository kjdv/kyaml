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
      d_out << val.get();
      indent();
    }

    void visit(sequence const &seq) override
    {
      ++d_indent;
      indent();
    }

    void visit(mapping const &map) override
    {
      ++d_indent;
      indent();
    }

    void sentinel(sequence const &seq) override
    {
      --d_indent;
      indent();
    }

    void sentinel(mapping const &map) override
    {
      --d_indent;
      indent();
    }

    void visit_key(size_t idx) override
    {
      d_out << "- ";
    }

    void visit_key(std::string const &key) override
    {
      d_out << key << ": ";
    }

  private:
    void indent()
    {
      d_out << '\n';
      if(d_indent >= 0)
      {
        for(int i = 0; i < d_indent; ++i)
          d_out << "  ";
      }
    }

    int d_indent;
    ostream &d_out;
  };
}

int main()
{
  kyaml::parser p(std::cin);
  unique_ptr<const document> doc = p.parse();

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

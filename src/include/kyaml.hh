#ifndef KYAML_HH
#define KYAML_HH

#include <memory>
#include <istream>
#include "node.hh"

namespace kyaml
{
  class parser_impl;
  class parser
  {
  public:
    parser(std::istream &input);
    ~parser();

    std::unique_ptr<const document> parse(); // may throw

  private:
    std::unique_ptr<parser_impl> d_pimpl; // trick to encapsulate dependencies
  };
}

#endif // KYAML_HH

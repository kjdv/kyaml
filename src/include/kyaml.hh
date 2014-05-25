#ifndef KYAML_HH
#define KYAML_HH

#include <memory>
#include <istream>
#include "node.hh"

namespace kyaml
{
  std::unique_ptr<const document> parse(std::istream &input);
}

#endif // KYAML_HH

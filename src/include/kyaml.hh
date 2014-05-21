#ifndef KYAML_HH
#define KYAML_HH

#include <memory>
#include <istream>
#include "node.hh"

namespace kyaml
{
  std::shared_ptr<document> parse(std::istream &input);
}

#endif // KYAML_HH

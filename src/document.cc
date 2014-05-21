#include "document.hh"
#include <sstream>

using namespace std;
using namespace kyaml;

namespace
{
  ostream &operator<<(ostream &o, document::type_t t)
  {
    switch(t)
    {
    case document::MAPPING:
      o << "mapping";
      break;
    case document::SEQUENCE:
      o << "sequence";
      break;
    case document::SCALAR:
      o << "scalar";
      break;
    default:
      o << "(invalid " << (int)t << ")";
    }
    return o;
  }
}

document::wrong_type::wrong_type(type_t expect, type_t actual)
{
  stringstream str;
  str << "node type mismatch: expected " << expect << " but is " << actual << '\n';
  d_msg = str.str();
}



sequence const &document::as_sequence() const
{
  if(type() != SEQUENCE)
    throw wrong_type(SEQUENCE, type());

  return dynamic_cast<sequence const &>(*this);
}

scalar const &document::as_scalar() const
{
  if(type() != SCALAR)
    throw wrong_type(SCALAR, type());

  return dynamic_cast<scalar const &>(*this);
}

mapping const &document::as_mapping() const
{
  if(type() != MAPPING)
    throw wrong_type(MAPPING, type());

  return dynamic_cast<mapping const &>(*this);
}

string const &document::get() const
{
  return as_scalar().get();
}

document const &document::get(size_t i) const
{
  return as_sequence().get(i);
}

document const &document::get(const string &key) const
{
  return as_mapping().get(key);
}


document const &mapping::get(const string &key) const
{
  container_t::const_iterator it = d_items.find(key);
  assert(it != d_items.end());
  assert(it->second);
  return *it->second;
}

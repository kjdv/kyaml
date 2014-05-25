#include "node.hh"
#include "node_visitor.hh"
#include "utils.hh"
#include <sstream>

using namespace std;
using namespace kyaml;

namespace
{
  ostream &operator<<(ostream &o, node::type_t t)
  {
    switch(t)
    {
    case node::MAPPING:
      o << "mapping";
      break;
    case node::SEQUENCE:
      o << "sequence";
      break;
    case node::SCALAR:
      o << "scalar";
      break;
    default:
      o << "(invalid " << (int)t << ")";
    }
    return o;
  }
}

node::wrong_type::wrong_type(type_t expect, type_t actual)
{
  stringstream str;
  str << "node type mismatch: expected " << expect << " but is " << actual << '\n';
  d_msg = str.str();
}



sequence const &node::as_sequence() const
{
  if(type() != SEQUENCE)
    throw wrong_type(SEQUENCE, type());

  return dynamic_cast<sequence const &>(*this);
}

scalar const &node::as_scalar() const
{
  if(type() != SCALAR)
    throw wrong_type(SCALAR, type());

  return dynamic_cast<scalar const &>(*this);
}

mapping const &node::as_mapping() const
{
  if(type() != MAPPING)
    throw wrong_type(MAPPING, type());

  return dynamic_cast<mapping const &>(*this);
}

string const &node::get() const
{
  assert(type() != SCALAR);
  throw wrong_type(SCALAR, type());
}

node const &node::get(size_t i) const
{
  assert(type() != SEQUENCE);
  throw wrong_type(SEQUENCE, type());
}

node const &node::get(const string &key) const
{
  assert(type() != MAPPING);
  throw wrong_type(MAPPING, type());
}

void node::add(std::shared_ptr<node> val)
{
  if(type() != SEQUENCE)
    throw wrong_type(SEQUENCE, type());

  dynamic_cast<sequence &>(*this).add(val);
}

void node::add(string const &key, std::shared_ptr<node> val)
{
  if(type() != MAPPING)
    throw wrong_type(MAPPING, type());

  return dynamic_cast<mapping &>(*this).add(key, val);
}

node const &mapping::get(const string &key) const
{
  container_t::const_iterator it = d_items.find(key);
  assert(it != d_items.end());
  assert(it->second);
  return *it->second;
}

const std::string scalar::s_null = "!!null";
const std::string scalar::s_bool = "!!bool";
const std::string scalar::s_integer = "!!int";
const std::string scalar::s_float = "!!float";
const std::string scalar::s_string = "!!str";
const std::string scalar::s_binary = "!!binary"; // base64-encoded binary

void scalar::accept(node_visitor &visitor) const
{
  visitor.visit(*this);
}

void sequence::accept(node_visitor &visitor) const
{
  visitor.visit(*this);
}

void mapping::accept(node_visitor &visitor) const
{
  visitor.visit(*this);
}

template<>
bool kyaml::type_convert(node::properties_t const &props, std::string const &input)
{
  string compare;
  for(char c : input)
  {
    if(isalnum(c)) // ignore non-alpha (ws), and convert to lower case
      compare += tolower(c);
  }

  for(std::string const &key : {"true", "yes", "t", "y"}) // positive
  {
    if(compare.compare(0, key.size(), key) == 0)
      return true;
  }

  for(std::string const &key : {"false", "no", "f", "n"}) // negative
  {
    if(compare.compare(0, key.size(), key) == 0)
      return false;
  }

  // undefined, revert to default
  return type_convert<int>(props, input);
}

template<>
vector<uint8_t> kyaml::type_convert(node::properties_t const &props, string const &input)
{
  // sanitize whitespace
  string sanitized;
  for(char c : input)
  {
    if(isalnum(c) || c == '+' || c == '/' || c == '=')
      sanitized += c;
  }

  vector<uint8_t> target;
  if(decode_base64(sanitized, target))
    return target;
  else
    return vector<uint8_t>();
}

namespace
{
  class node_printer : public node_visitor
  {
  public:
    node_printer(ostream &out) :
      d_out(out)
    {}

    void visit(scalar const &val) override
    {
      d_out << val.get();
    }

    void visit(sequence const &seq) override
    {
      bool first = true;

      d_out << "[";
      for(auto const &item : seq)
      {
        if(first)
          first = false;
        else
          d_out << ", ";

        item->accept(*this);
      }
      d_out << "]";
    }

    void visit(mapping const &map) override
    {
      bool first = true;

      d_out << "{";
      for(auto const &item : map)
      {
        if(first)
          first = false;
        else
          d_out << ", ";

        d_out << item.first << ": ";
        item.second->accept(*this);
      }
      d_out << "}";
    }

  private:
    ostream &d_out;
  };
}

ostream &std::operator<<(ostream &out, kyaml::node const &node)
{
  node_printer np(out);
  node.accept(np);

  return out;
}


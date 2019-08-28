#include "node.hh"
#include "node_visitor.hh"
#include "utils.hh"
#include <sstream>

using namespace std;
using namespace kyaml;

namespace std
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

namespace
{

  inline void throw_type_error(node::type_t expect, node::type_t actual)
  {
    throw node::type_error(
          string("node type mismatch: expected ") +
          tostring_cast(expect) +
          " but was " +
          tostring_cast(actual));
  }
}

sequence const &node::as_sequence() const
{
  if(type() != SEQUENCE)
    throw_type_error(SEQUENCE, type());

  return dynamic_cast<sequence const &>(*this);
}

scalar const &node::as_scalar() const
{
  if(type() != SCALAR)
    throw_type_error(SCALAR, type());

  return dynamic_cast<scalar const &>(*this);
}

mapping const &node::as_mapping() const
{
  if(type() != MAPPING)
    throw_type_error(MAPPING, type());

  return dynamic_cast<mapping const &>(*this);
}

string const &node::get() const
{
  assert(type() != SCALAR);
  throw_type_error(SCALAR, type());
  throw std::logic_error("never reached");
}

node const &node::get(size_t i) const
{
  assert(type() != SEQUENCE);
  throw_type_error(SEQUENCE, type());
  throw std::logic_error("never reached");
}

node const &node::get(const string &key) const
{
  assert(type() != MAPPING);
  throw_type_error(MAPPING, type());
  throw std::logic_error("never reached");
}

void node::add(std::shared_ptr<node> val)
{
  if(type() != SEQUENCE)
    throw_type_error(SEQUENCE, type());

  dynamic_cast<sequence &>(*this).add(val);
}

void node::add(string const &key, std::shared_ptr<node> val)
{
  if(type() != MAPPING)
    throw_type_error(MAPPING, type());

  return dynamic_cast<mapping &>(*this).add(key, val);
}

node const &mapping::operator[](const string &key) const
{
  container_t::const_iterator it = d_items.find(key);
  assert(it != d_items.end());
  assert(it->second);
  return *it->second;
}

node const &mapping::get(const string &key) const
{
  container_t::const_iterator it = d_items.find(key);
  if(it == d_items.end())
    throw value_error(string("requested value ") + key + " not found");

  assert(it->second);
  return *it->second;
}

const string scalar::null_property = "!!null";
const string scalar::bool_property = "!!bool";
const string scalar::int_property = "!!int";
const string scalar::float_property = "!!float";
const string scalar::string_property = "!!str";
const string scalar::binary_property = "!!binary";

void scalar::accept(node_visitor &visitor) const
{
  visitor.visit(*this);
}

void sequence::accept(node_visitor &visitor) const
{
  visitor.visit(*this);

  for(size_t idx = 0; idx < size(); ++idx)
  {
    assert(d_items[idx]);

    visitor.visit_key(idx);
    d_items[idx]->accept(visitor);
  }

  visitor.sentinel(*this);
}

node const &sequence::get(size_t i) const
{
  if(i >= size())
    throw value_error(string("list index ") + tostring_cast(i) + " out of range");
  assert(d_items[i]);
  return *d_items[i];
}

void mapping::accept(node_visitor &visitor) const
{
  visitor.visit(*this);

  for(auto&& kvpair : d_items)
  {
    assert(kvpair.second);

    visitor.visit_key(kvpair.first);
    kvpair.second->accept(visitor);
  }

  visitor.sentinel(*this);
}

template<>
bool kyaml::type_convert(node::properties_t const &props, std::string const &input)
{
  string sanitized;
  for(char c : input)
  {
    if(isalnum(c)) // ignore non-alpha (ws), and convert to lower case
      sanitized += tolower(c);
  }

  for(std::string const &key : {"true", "yes", "t", "y"}) // positive
  {
    if(sanitized.compare(0, key.size(), key) == 0)
      return true;
  }

  for(std::string const &key : {"false", "no", "f", "n"}) // negative
  {
    if(sanitized.compare(0, key.size(), key) == 0)
      return false;
  }

  // undefined, revert to default
  return type_convert<int>(props, input);
}

template<>
binary_t kyaml::type_convert(node::properties_t const &props, string const &input)
{
  // sanitize whitespace
  string sanitized;
  for(char c : input)
  {
    if(isalnum(c) || c == '+' || c == '/' || c == '=')
      sanitized += c;
  }

  binary_t target;
  if(decode_base64(sanitized, target))
    return target;
  else
    return binary_t();
}

namespace
{
  class node_printer : public node_visitor
  {
  public:
    node_printer(ostream &out) :
      d_needscomma(false),
      d_out(out)
    {}

    void visit(scalar const &val) override
    {
      comma();
      d_out << val.get();
      needscomma(true);
    }

    void visit(sequence const &seq) override
    {
      comma();
      d_out << "[";
      needscomma(false);
    }

    void visit(mapping const &map) override
    {
      comma();
      d_out << "{";
      needscomma(false);
    }

    void sentinel(sequence const &seq) override
    {
      d_out << "]";
      needscomma(true);
    }

    void sentinel(mapping const &map) override
    {
      d_out << "}";
      needscomma(true);
    }

    void visit_key(std::string const &key) override
    {
      comma();
      d_out << key << ": ";
      needscomma(false);
    }

  private:
    void comma()
    {
      if(d_needscomma)
        d_out << ", ";
    }

    void needscomma(bool v)
    {
      d_needscomma = v;
    }

    bool d_needscomma;
    ostream &d_out;
  };
}

ostream &std::operator<<(ostream &out, kyaml::node const &node)
{
  node_printer np(out);
  node.accept(np);

  return out;
}


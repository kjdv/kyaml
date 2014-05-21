#include "node_builder.hh"

using namespace std;
using namespace kyaml;

void node_builder::start_sequence()
{
  d_log("starting sequence");
  d_stack.emplace(SEQUENCE, make_shared<sequence>());
}

void node_builder::end_sequence()
{
  resolve();
  d_log("completed sequence ", d_stack.top().value);
}

void node_builder::start_mapping()
{
  d_log("start mapping");
  d_stack.emplace(MAPPING, make_shared<mapping>());
}

void node_builder::end_mapping()
{
  resolve();
  d_log("completed mapping ", d_stack.top().value);
}

void node_builder::add_anchor(const string &anchor)
{
  d_log("anchor ", anchor);
  assert(false); // TODO
}

void node_builder::add_alias(const string &alias)
{
  d_log("alias ", alias);
  assert(false); // TODO
}

void node_builder::add_scalar(const string &val)
{
  d_log("scalar", val);

  shared_ptr<scalar> s = make_shared<scalar>(val);

  if(d_stack.empty())
  {
    d_log("bare");
    d_stack.emplace(RESOLVED_NODE, s);
  }
  else
  {
    item &it = d_stack.top();
    switch(it.token)
    {
    case SEQUENCE:
      d_log("adding to sequence");
      it.value->add(s);
      break;

    case MAPPING:
      d_log("using as key");
      d_stack.emplace(MAPPING_KEY, s);
      break;

    case MAPPING_KEY:
    {
      d_log("using as value");
      item key = pop();
      assert(!d_stack.empty() && d_stack.top().token == MAPPING);
      d_stack.top().value->add(key.value->get(), s);
      break;
    }

    default:
      assert(false);
    }
  }
}

void node_builder::add_atom(char32_t c)
{
  // d_log("atom (?) ", c);
  // assert(false); // should not leak to this level
}

shared_ptr<node> node_builder::build()
{
  assert(d_stack.size() == 1);
  assert(d_stack.top().token == RESOLVED_NODE);

  d_log("building", d_stack.top().value);
  return d_stack.top().value;
}

node_builder::item node_builder::pop()
{
  assert(!d_stack.empty());
  item it = d_stack.top();
  d_stack.pop();
  return it;
}

void node_builder::resolve()
{
  d_stack.top().token = RESOLVED_NODE;
}

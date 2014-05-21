#include "node_builder.hh"

using namespace std;
using namespace kyaml;

void node_builder::start_sequence()
{
  d_log("starting sequence");
  d_stack.emplace(SEQUENCE_START);
}

void node_builder::end_sequence()
{
  shared_ptr<sequence> sq;

  item it = pop();
  while(it.token != SEQUENCE_START)
  {
    assert(it.token == RESOLVED_NODE);
    sq->reverse_add(it.value);

    it = pop();
  }

  d_log("completed sequence ", sq);
  d_stack.emplace(RESOLVED_NODE, sq);
}

void node_builder::start_mapping()
{
  d_log("start mapping");
  d_stack.emplace(MAPPING_START);
}

void node_builder::end_mapping()
{
  shared_ptr<mapping> mp;

  item value = pop();
  while(value.token != MAPPING_START)
  {
    item key = pop();

    assert(key.token == RESOLVED_NODE);
    assert(key.value->type() == node::SCALAR);

    mp->add(key.value->get(), value.value);

    value = pop();
  }

  d_log("completed mapping ", mp);
  d_stack.emplace(RESOLVED_NODE, mp);
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
  d_log("scalar ", val);
  d_stack.emplace(RESOLVED_NODE, make_shared<scalar>(val));
}

void node_builder::add_atom(char32_t c)
{
  assert(false); // should not leak to this level
}

shared_ptr<node> node_builder::build()
{
  assert(d_stack.size() == 1);
  assert(d_stack.top().token == RESOLVED_NODE);

  d_log("building ", d_stack.top().value);
  return d_stack.top().value;
}

node_builder::item node_builder::pop()
{
  assert(!d_stack.empty());
  item it = d_stack.top();
  d_stack.pop();
  return it;
}

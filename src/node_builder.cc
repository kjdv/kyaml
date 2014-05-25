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
  d_log("ending sequence");
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
  d_log("ending mapping");
  resolve();
  d_log("completed mapping ", d_stack.top().value);
}

void node_builder::add_anchor(const string &anchor)
{
  d_log("anchor", anchor);
  d_stack.emplace(ANCHOR, make_shared<scalar>(anchor));
}

void node_builder::add_alias(const string &alias)
{
  d_log("alias", alias);

  unordered_map<string, weak_ptr<node> >::const_iterator it = d_anchors.find(alias);
  if(it != d_anchors.end())
  {
    shared_ptr<node> sp = it->second.lock();
    if(sp)
    {
      add_resolved_node(sp);
      return;
    }
  }
  throw unkown_alias(alias);
}

void node_builder::add_scalar(const string &val)
{
  d_log("scalar", val);

  shared_ptr<scalar> s = make_shared<scalar>(val);

  add_resolved_node(s);
}

void node_builder::add_resolved_node(shared_ptr<node> s)
{
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
    case ANCHOR:
    {
      item key = pop();
      d_log("storing anchor", key.value->get(), s);
      d_anchors.insert(make_pair(key.value->get(), s));
      add_resolved_node(s);
      break;
    }

    default:
      assert(false);
    }
  }
}

void node_builder::add_atom(char32_t c)
{
  // TODO: sometimes called, but shouldn't be. ignore for now
  // d_log("atom (?) ", c);
  // assert(false); // should not leak to this level
}

unique_ptr<node> node_builder::build()
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
  assert(!d_stack.empty());
  shared_ptr<node> rn = d_stack.top().value;
  d_stack.pop();

  add_resolved_node(rn);
}

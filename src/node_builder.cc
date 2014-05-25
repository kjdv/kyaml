#include "node_builder.hh"

using namespace std;
using namespace kyaml;

namespace
{
  template <typename t>
  void dont_delete(t *p)
  {}
}

namespace std
{
  ostream &operator<<(ostream &o, shared_ptr<node> n)
  {
    if(n)
      n->print(o);
    else
      o << "(nullptr)";
    return o;
  }
}

void node_builder::start_sequence()
{
  d_log("starting sequence");
  push(SEQUENCE, std::unique_ptr<sequence>(new sequence));
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
  push(MAPPING, std::unique_ptr<mapping>(new mapping));
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
  push(ANCHOR, std::unique_ptr<scalar>(new scalar(anchor)));
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

  if(d_stack.empty())
    push(RESOLVED_NODE, unique_ptr<scalar>(new scalar(val)));
  else
  {
    shared_ptr<scalar> s = make_shared<scalar>(val);
    add_resolved_node(s);
  }
}

void node_builder::add_resolved_node(shared_ptr<node> s)
{
  if(d_stack.empty())
  {
    d_log("bare");
    push_shared(RESOLVED_NODE, s);
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
      push_shared(MAPPING_KEY, s);
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

void node_builder::push(node_builder::token_t t, std::unique_ptr<node> v)
{
  if(d_stack.empty())
  {
    // the top element in the stack is owned by d_root, not d_stack, so put shared_ptr with a no-op delete on top
    d_root = std::move(v);
    std::shared_ptr<node> sp(d_root.get(), dont_delete<node>);
    d_stack.emplace(t, sp);
  }
  else
    d_stack.emplace(t, std::move(v));
}

void node_builder::push_shared(token_t t, std::shared_ptr<node> v)
{
  assert(!d_stack.empty());
  d_stack.emplace(t, v);
}

void node_builder::add_atom(char32_t c)
{
  // TODO: sometimes called, but shouldn't be. ignore for now
  // d_log("atom (?) ", c);
  // assert(false); // should not leak to this level
}

unique_ptr<node> node_builder::build()
{
  assert(d_root);
  assert(d_stack.size() == 1);
  assert(d_stack.top().token == RESOLVED_NODE);

  d_log("building", d_stack.top().value);

  d_stack = stack<item>();
  return std::move(d_root);
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

  if(d_stack.size() == 1)
    d_stack.top().token = RESOLVED_NODE;
  else
  {
    shared_ptr<node> rn = d_stack.top().value;
    d_stack.pop();

    add_resolved_node(rn);
  }
}

#include "node_builder.hh"

using namespace std;
using namespace kyaml;

namespace
{
  template <typename t>
  void dont_delete(t *p)
  {}

  class properties_node : public node
  {
  public:
    type_t type() const
    {
      return SCALAR;
    }

    void accept(node_visitor &visitor) const
    {}
  };
}

namespace std
{
  ostream &operator<<(ostream &o, shared_ptr<node> n)
  {
    if(n)
    {
      node const &nr = *n;
      o << nr;
    }
    else
      o << "(nullptr)";
    return o;
  }
}

void node_builder::start_sequence(context const &ctx)
{
  d_log("starting sequence");
  push(SEQUENCE, ctx, std::unique_ptr<sequence>(new sequence));
}

void node_builder::end_sequence(context const &ctx)
{
  d_log("ending sequence");
  resolve();
  d_log("completed sequence ", d_stack.top().value);
}

void node_builder::start_mapping(context const &ctx)
{
  d_log("start mapping");
  push(MAPPING, ctx, std::unique_ptr<mapping>(new mapping));
}

void node_builder::end_mapping(context const &ctx)
{
  d_log("ending mapping");
  resolve();
  d_log("completed mapping ", d_stack.top().value);
}

void node_builder::add_anchor(context const &ctx, const string &anchor)
{
  d_log("anchor", anchor);
  push(ANCHOR, ctx, std::unique_ptr<scalar>(new scalar(anchor)));
}

void node_builder::add_alias(context const &ctx, const string &alias)
{
  d_log("alias", alias);

  unordered_map<string, weak_ptr<node> >::const_iterator it = d_anchors.find(alias);
  if(it != d_anchors.end())
  {
    shared_ptr<node> sp = it->second.lock();
    if(sp)
    {
      add_resolved_node(ctx, sp);
      return;
    }
  }

  d_errors.emplace_back(ctx, string("unknown alias '" + alias + "'"));
}

void node_builder::add_scalar(context const &ctx, const string &val)
{
  d_log("scalar", val);

  if(d_stack.empty())
    push(RESOLVED_NODE, ctx, unique_ptr<scalar>(new scalar(val)));
  else
  {
    shared_ptr<scalar> s = make_shared<scalar>(val);
    add_resolved_node(ctx, s);
  }
}

void node_builder::add_property(context const &ctx, string const &prop)
{
  d_log("propery", prop);

  if(d_stack.empty() ||  d_stack.top().token != PROPERTY)
    d_stack.emplace(PROPERTY, ctx, std::unique_ptr<properties_node>(new properties_node));

  d_stack.top().value->add_property(prop);
}

void node_builder::add_resolved_node(context const &ctx, shared_ptr<node> s)
{
  if(d_stack.empty())
  {
    d_log("bare");
    push_shared(RESOLVED_NODE, ctx, s);
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
      push_shared(MAPPING_KEY, ctx, s);
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
      add_resolved_node(ctx, s); // or key.ctx?
      break;
    }
    case PROPERTY:
    {
      item props = pop();
      for(std::string const &p : props.value->properties())
        s->add_property(p);
      add_resolved_node(ctx, s); // or props.ctx?
      break;
    }

    default:
      assert(false);
    }
  }
}

void node_builder::push(node_builder::token_t t, context const &ctx, std::unique_ptr<node> v)
{
  if(d_stack.empty())
  {
    // the top element in the stack is owned by d_root, not d_stack, so put shared_ptr with a no-op delete on top
    d_root = std::move(v);
    std::shared_ptr<node> sp(d_root.get(), dont_delete<node>);
    d_stack.emplace(t, ctx, sp);
  }
  else
    d_stack.emplace(t, ctx, std::move(v));
}

void node_builder::push_shared(token_t t, context const &ctx, std::shared_ptr<node> v)
{
  assert(!d_stack.empty());
  d_stack.emplace(t, ctx, v);
}

void node_builder::add_atom(context const &ctx, char32_t c)
{
  // TODO: sometimes called, but shouldn't be. ignore for now
  // d_log("atom (?) ", c);
  // assert(false); // should not leak to this level
}

unique_ptr<node> node_builder::build()
{
  struct cleaner
  {
    node_builder &subject;

    cleaner(node_builder &s) :
      subject(s)
    {}
    ~cleaner()
    {
      subject.clear();
    }
  };

  cleaner cl(*this);

  if(!d_root || d_stack.empty())
    return unique_ptr<node>(new scalar(""));

  if(!d_errors.empty())
  {
    error const &err = d_errors.front();
    stringstream str;
    str << "Content error at line " << err.ctx.linenumber();
    if(!err.msg.empty())
      str << ": " << err.msg;

    throw content_error(err.ctx.linenumber(), str.str());
  }

  assert(d_stack.size() == 1);
  assert(d_stack.top().token == RESOLVED_NODE);

  d_log("building", d_stack.top().value);

  return std::move(d_root);
}

void node_builder::clear()
{
  d_stack = stack<item>();
  d_errors.clear();
  d_root.reset();
  d_anchors.clear();
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

  d_log("resolving", d_stack.top().value);

  if(d_stack.size() == 1)
    d_stack.top().token = RESOLVED_NODE;
  else
  {
    shared_ptr<node> rn = d_stack.top().value;
    context ctx = d_stack.top().ctx;
    d_stack.pop();

    add_resolved_node(ctx, rn);
  }
}

#ifndef NODE_BUILDER_HH
#define NODE_BUILDER_HH

#include "node.hh"
#include "kyaml.hh"
#include "document_builder.hh"
#include <stack>

namespace kyaml
{
  class node_builder : public document_builder
  {
  public:
    typedef kyaml::parser::content_error content_error;

    node_builder() :
      d_log("node builder")
    {}

    void start_sequence(context const &ctx) override;

    void end_sequence(context const &ctx) override;

    void start_mapping(context const &ctx) override;

    void end_mapping(context const &ctx) override;

    void add_anchor(context const &ctx, std::string const &anchor) override;

    void add_alias(context const &ctx, std::string const &alias) override;

    void add_scalar(context const &ctx, std::string const &val) override;

    void add_property(context const &ctx, std::string const &prop) override;

    void add_atom(context const &ctx, char32_t c) override;

    // may throw
    std::unique_ptr<node> build();

    void clear();

  private:
    typedef enum
    {
      SEQUENCE,
      MAPPING,
      MAPPING_KEY,
      ANCHOR,
      PROPERTY,
      RESOLVED_NODE
    } token_t;

    struct item
    {
      token_t token;
      context ctx;
      std::shared_ptr<node> value;

      item(token_t t, context const &c, std::shared_ptr<node> v = std::shared_ptr<node>()) :
        token(t),
        ctx(c),
        value(v)
      {}
    };

    item pop();

    void resolve();
    void add_resolved_node(context const &ctx, std::shared_ptr<node> s);

    void push(token_t t, context const &ctx, std::unique_ptr<node> v = std::unique_ptr<node>());
    void push_shared(token_t t, context const &ctx, std::shared_ptr<node> v);

    std::unordered_map<std::string, std::weak_ptr<node> > d_anchors;

    struct error
    {
      context ctx;
      std::string msg;

      error(context const &c, std::string const &m) :
        ctx(c),
        msg(m)
      {}
    };
    std::vector<error> d_errors;

    std::stack<item> d_stack;
    std::unique_ptr<node> d_root;

    logger<false> d_log;
  };
}

#endif // NODE_BUILDER_HH

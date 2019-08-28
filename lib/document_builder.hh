#ifndef DOCUMENT_BUILDER_HH
#define DOCUMENT_BUILDER_HH

#include <string>
#include <ostream>
#include <memory>
#include <utils.hh>
#include <cassert>
#include <vector>

namespace kyaml
{
  class context;

  class document_builder
  {
  public:
    // in purpose much like the global context, except this explicitely has no access to
    // and cannot modify the stream, and has to be a copyable type
    class context
    {
    public:
      context(kyaml::context const &ctx);

      unsigned linenumber() const
      {
        return d_linenumber;
      }
    private:
      unsigned d_linenumber;
    };

    virtual ~document_builder()
    {}

    virtual void start_sequence(context const &ctx) = 0;
    virtual void end_sequence(context const &ctx) = 0;
    virtual void start_mapping(context const &ctx) = 0;
    virtual void end_mapping(context const &ctx) = 0;

    virtual void add_anchor(context const &ctx, std::string const &anchor) = 0;
    virtual void add_alias(context const &ctx, std::string const &alias) = 0;
    virtual void add_scalar(context const &ctx, std::string const &val) = 0;
    virtual void add_atom(context const &ctx, char32_t c) = 0;
    virtual void add_property(context const &ctx, std::string const &prop) = 0;
  };

  class string_builder : public document_builder
  {
  public:
    void start_sequence(context const &ctx) override
    {}

    void end_sequence(context const &ctx) override
    {}

    void start_mapping(context const &ctx) override
    {}

    void end_mapping(context const &ctx) override
    {}

    void add_anchor(context const &ctx, std::string const &) override
    {}

    void add_alias(context const &ctx, std::string const &) override
    {}

    void add_property(context const &ctx, std::string const &) override
    {}

    void add_scalar(context const &ctx, std::string const &val) override
    {
      d_value += val;
    }

    void add_atom(context const &ctx, char32_t c) override
    {
      append_utf8(d_value, c);
    }

    std::string const &build() const
    {
      return d_value;
    }

  private:
    std::string d_value;
  };

  class null_builder : public document_builder
  {
  public:
    void start_sequence(context const &ctx) override
    {}

    void end_sequence(context const &ctx) override
    {}

    void start_mapping(context const &ctx) override
    {}

    void end_mapping(context const &ctx) override
    {}

    void add_anchor(context const &ctx, std::string const &) override
    {}

    void add_alias(context const &ctx, std::string const &) override
    {}

    void add_scalar(context const &ctx, std::string const &) override
    {}

    void add_property(context const &ctx, std::string const &) override
    {}

    void add_atom(context const &ctx, char32_t c) override
    {}
  };

  class replay_builder : public document_builder
  {
  public:

    void start_sequence(context const &ctx) override;
    void end_sequence(context const &ctx) override;
    void start_mapping(context const &ctx) override;
    void end_mapping(context const &ctx) override;

    void add_anchor(context const &ctx, std::string const &anchor) override;
    void add_alias(context const &ctx, std::string const &alias) override;
    void add_scalar(context const &ctx, std::string const &val) override;
    void add_atom(context const &ctx, char32_t c) override;
    void add_property(context const &ctx, std::string const &prop) override;

    void replay(document_builder &builder) const;

  private:
    typedef enum
    {
      START_SEQUENCE,
      END_SEQUENCE,
      START_MAPPING,
      END_MAPPING,
      ANCHOR,
      ALIAS,
      SCALAR,
      ATOM,
      PROPERTY,
    } token_t;

    struct item
    {
      token_t token;
      context ctx;
      std::string value;
      char32_t atom;

      item(token_t t, context const &c, std::string const &v = "") :
        token(t),
        ctx(c),
        value(v),
        atom(0)
      {}

      item(token_t t, context const &c, char32_t ch) :
        token(t),
        ctx(c),
        atom(ch)
      {}
    };

    std::vector<item> d_items;
  };
}

#endif // DOCUMENT_BUILDER_HH

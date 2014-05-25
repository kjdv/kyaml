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
  class document_builder
  {
  public:
    virtual ~document_builder()
    {}

    virtual void start_sequence() = 0;
    virtual void end_sequence() = 0;
    virtual void start_mapping() = 0;
    virtual void end_mapping() = 0;

    virtual void add_anchor(std::string const &anchor) = 0;
    virtual void add_alias(std::string const &alias) = 0;
    virtual void add_scalar(std::string const &val) = 0;
    virtual void add_atom(char32_t c) = 0;
    virtual void add_property(std::string const &prop) = 0;
  };

  class string_builder : public document_builder
  {
  public:
    void start_sequence() override
    {}

    void end_sequence() override
    {}

    void start_mapping() override
    {}

    void end_mapping() override
    {}

    void add_anchor(std::string const &) override
    {}

    void add_alias(std::string const &) override
    {}

    void add_property(std::string const &) override
    {}

    void add_scalar(std::string const &val) override
    {
      d_value += val;
    }

    void add_atom(char32_t c) override
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
    void start_sequence() override
    {}

    void end_sequence() override
    {}

    void start_mapping() override
    {}

    void end_mapping() override
    {}

    void add_anchor(std::string const &) override
    {}

    void add_alias(std::string const &) override
    {}

    void add_scalar(std::string const &) override
    {}

    void add_property(std::string const &) override
    {}

    void add_atom(char32_t c) override
    {}
  };

  class replay_builder : public document_builder
  {
  public:

    void start_sequence() override;
    void end_sequence() override;
    void start_mapping() override;
    void end_mapping() override;

    void add_anchor(std::string const &anchor) override;
    void add_alias(std::string const &alias) override;
    void add_scalar(std::string const &val) override;
    void add_atom(char32_t c) override;
    void add_property(std::string const &prop) override;

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
      std::string value;
      char32_t atom;

      item(token_t t, std::string const &v = "") :
        token(t),
        value(v),
        atom(0)
      {}

      item(token_t t, char32_t c) :
        token(t),
        atom(c)
      {}
    };

    std::vector<item> d_items;
  };
}

#endif // DOCUMENT_BUILDER_HH

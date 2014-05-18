#ifndef DOCUMENT_BUILDER_HH
#define DOCUMENT_BUILDER_HH

#include <string>
#include <ostream>
#include <memory>
#include <utils.hh>
#include <cassert>

namespace kyaml
{
  class document_builder
  {
  public:
    virtual ~document_builder()
    {}

    // todo: make pure virtual
    virtual void add_anchor(std::string const &anchor)
    {}
    virtual void add_alias(std::string const &alias)
    {}
    virtual void add_scalar(std::string const &val)
    {} 
    virtual void add_atom(char32_t c)
    {}
  };

  class string_builder : public document_builder
  {
  public:
    void add_anchor(std::string const &anchor) override
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

  class dummy_document_builder : public document_builder
  {
  public:
  };
}

#endif // DOCUMENT_BUILDER_HH

#ifndef DOCUMENT_BUILDER_HH
#define DOCUMENT_BUILDER_HH

#include <string>
#include <ostream>
#include <memory>
#include <utils.hh>
#include <cassert>

namespace kyaml
{
  class void_item
  {
  public:
    void_item()
    {}
    template <typename T>
    void_item(T const &)
    {}
    
    bool operator==(void_item const &) const
    {
      return true;
    }
  };

  class document_builder
  {
  public:
    virtual ~document_builder()
    {}

    virtual void add(char const *tag, void_item const &v) = 0;
    virtual void add(char const *tag, std::string const &v) = 0;
    void add(char const *tag, char32_t c)
    {
      std::string s;
      append_utf8(s, c);
      add(tag, s);
    }    

    virtual void add_anchor(std::string const &anchor) = 0;
    virtual void add_scalar(std::string const &val)
    {} // todo: make pure virtual
  };

  class string_builder : public document_builder
  {
  public:
    void add(char const *tag, void_item const &v) override
    {}

    void add(char const *tag, std::string const &v) override
    {
      d_value.append(v);
    }

    void add_anchor(std::string const &anchor) override
    {}

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
    void add(char const *tag, void_item const &v) override
    {}
    void add(char const *tag, std::string const &v) override
    {}

    void add_anchor(std::string const &anchor) override
    {}
  };
}

namespace std
{
  inline ostream &operator<<(ostream &out, kyaml::void_item const &v)
  {
    return out << "(void)";
  }
}

#endif // DOCUMENT_BUILDER_HH

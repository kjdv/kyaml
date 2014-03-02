#ifndef DOCUMENT_BUILDER_HH
#define DOCUMENT_BUILDER_HH

#include <string>
#include <ostream>
#include <memory>
#include <utils.hh>

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
    typedef std::unique_ptr<document_builder> child_t;

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

    virtual child_t child() = 0;
    virtual void add(char const *tag, child_t c);
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

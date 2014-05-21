#ifndef DOCUMENT_HH
#define DOCUMENT_HH

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <cassert>
#include <sstream>
#include <utils.hh>

namespace kyaml
{
  class sequence;
  class mapping;
  class scalar;

  class document : private no_copy
  {
  public:
    typedef enum
    {
      SEQUENCE,
      MAPPING,
      SCALAR
    } type_t;

    class wrong_type : public std::exception
    {
    public:
      wrong_type(type_t expect, type_t actual);

      char const *what() const throw() override
      {
        return d_msg.c_str();
      }

    private:
      std::string d_msg;
    };

    virtual ~document()
    {}

    virtual type_t type() const = 0;

    // these will throw on type mismatch
    sequence const &as_sequence() const;

    scalar const &as_scalar() const;

    mapping const &as_mapping() const;

    // easy access: get as value, item from sequence, or value from map. may throw
    std::string const &get() const;

    document const &get(size_t i) const;

    document const &get(std::string const &key) const;
  };

  class scalar : public document
  {
  public:
    scalar(std::string const &v) :
      d_value(v)
    {}

    type_t type() const final
    {
      return SCALAR;
    }

    std::string const &get() const
    {
      return d_value;
    }

    template <typename T>
    T get() const
    {
      T t = T();
      std::stringstream str(d_value);
      str >> t;
      return t;
    }

  private:
    std::string d_value;
  };

  class sequence : public document
  {
  public:
    typedef std::vector<std::unique_ptr<document> > container_t;

    type_t type() const final
    {
      return SEQUENCE;
    }

    document const &operator[](size_t i) const
    {
      return get(i);
    }

    document const &get(size_t i) const
    {
      assert(i < size() && d_items[i]);
      return *d_items[i];
    }

    container_t::const_iterator begin() const
    {
      return d_items.begin();
    }

    container_t::const_iterator end() const
    {
      return d_items.end();
    }

    size_t size() const
    {
      return d_items.size();
    }

    void add(std::unique_ptr<document> child)
    {
      d_items.push_back(std::move(child));
    }

  private:
    container_t d_items;
  };

  class mapping : public document
  {
  public:
    typedef std::unordered_map<std::string, std::unique_ptr<document> > container_t;

    type_t type() const final
    {
      return MAPPING;
    }

    document const &operator[](std::string const &key) const
    {
      return get(key);
    }

    document const &get(std::string const &key) const;

    bool has(std::string const &key) const
    {
      return d_items.find(key) != d_items.end();
    }

    container_t::const_iterator begin() const
    {
      return d_items.begin();
    }

    container_t::const_iterator end() const
    {
      return d_items.end();
    }

    size_t size() const
    {
      return d_items.size();
    }

    void add(std::string const &key, std::unique_ptr<document> value)
    {
      d_items.insert(std::make_pair(key, std::move(value)));
    }

  private:
    container_t d_items;
  };
}

#endif // DOCUMENT_HH

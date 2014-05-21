#ifndef NODE_HH
#define NODE_HH

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

  class node : private no_copy
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

    virtual ~node()
    {}

    virtual type_t type() const = 0;

    virtual void print(std::ostream &o) const = 0;

    // these will throw on type mismatch
    sequence const &as_sequence() const;

    scalar const &as_scalar() const;

    mapping const &as_mapping() const;

    // easy access: get as value, item from sequence, or value from map. may throw
    std::string const &get() const;

    node const &get(size_t i) const;

    node const &get(std::string const &key) const;

    void add(std::shared_ptr<node> val);

    void add(std::string const &key, std::shared_ptr<node> val);
  };

  typedef node document;

  class scalar : public node
  {
  public:
    scalar(std::string const &v) :
      d_value(v)
    {}

    type_t type() const final
    {
      return SCALAR;
    }

    void print(std::ostream &o) const final
    {
      o << get();
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

  class sequence : public node
  {
  public:
    typedef std::vector<std::shared_ptr<node> > container_t;

    type_t type() const final
    {
      return SEQUENCE;
    }

    void print(std::ostream &o) const final;

    node const &operator[](size_t i) const
    {
      return get(i);
    }

    node const &get(size_t i) const
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

    void add(std::shared_ptr<node> child)
    {
      d_items.push_back(child);
    }

  private:
    container_t d_items;
  };

  class mapping : public node
  {
  public:
    typedef std::unordered_map<std::string, std::shared_ptr<node> > container_t;

    type_t type() const final
    {
      return MAPPING;
    }

    void print(std::ostream &o) const final;

    node const &operator[](std::string const &key) const
    {
      return get(key);
    }

    node const &get(std::string const &key) const;

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

    void add(std::string const &key, std::shared_ptr<node> value)
    {
      d_items.insert(std::make_pair(key, value));
    }

  private:
    container_t d_items;
  };
}

namespace std
{
  inline ostream &operator<<(ostream &o, kyaml::node const &n)
  {
    n.print(o);
    return o;
  }

  ostream &operator<<(ostream &o, std::shared_ptr<kyaml::node> sp);
}

#endif // NODE_HH

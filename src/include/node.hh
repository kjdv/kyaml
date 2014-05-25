#ifndef NODE_HH
#define NODE_HH

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <cassert>
#include <sstream>

namespace kyaml
{
  class sequence;
  class mapping;
  class scalar;

  class node
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

    // access child documents. Use integer-types to access items in sequence, strings as keys in mappings
    // will throw on type mismatch.

    node const &value() const
    {
      return *this;
    }
    template <typename... path_t>
    node const &value(size_t idx, path_t... path) const;
    template <typename... path_t>
    node const &value(std::string const &key, path_t... path) const;

    // check membership of the item
    bool has() const // just the sentinel
    {
      return true;
    }
    template <typename... path_t>
    bool has(size_t idx, path_t... path) const;
    template <typename... path_t>
    bool has(std::string const &key, path_t... path) const;

    // specialized value()/has() for leaf nodes
    template <typename... path_t>
    std::string const &leaf_value(path_t... path) const;

    template <typename... path_t>
    bool has_leaf(path_t... path) const;
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
    typedef std::vector<std::shared_ptr<const node> > container_t;

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

    void add(std::shared_ptr<const node> child)
    {
      d_items.push_back(child);
    }

  private:
    container_t d_items;
  };

  class mapping : public node
  {
  public:
    typedef std::unordered_map<std::string, std::shared_ptr<const node> > container_t;

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

    bool has_key(std::string const &key) const
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

    void add(std::string const &key, std::shared_ptr<const node> value)
    {
      d_items.insert(std::make_pair(key, value));
    }

  private:
    container_t d_items;
  };

  template <typename... path_t>
  const node &node::value(size_t idx, path_t... path) const
  {
    return as_sequence().get(idx).value(path...);
  }

  template <typename... path_t>
  const node &node::value(const std::string &key, path_t... path) const
  {
    return as_mapping().get(key).value(path...);
  }

  template <typename... path_t>
  bool node::has(size_t idx, path_t... path) const
  {
    if(type() == SEQUENCE)
    {
      sequence const &seq = as_sequence();
      return
        idx < seq.size() &&
        seq.get(idx).has(path...);
    }
    return false;
  }

  template <typename... path_t>
  bool node::has(std::string const &key, path_t... path) const
  {
    if(type() == MAPPING)
    {
      mapping const &map = as_mapping();
      return // todo: optimize, this requires 2 searches in the map
        map.has_key(key) &&
        map.get(key).has(path...);
    }
    return false;
  }

  template <typename... path_t>
  std::string const &node::leaf_value(path_t... path) const
  {
    return value(path...).as_scalar().get();
  }

  template <typename... path_t>
  bool node::has_leaf(path_t... path) const
  {
    return
      has(path...) && // todo: optimize. this requires 2 searches
      value(path...).type() == SCALAR;
  }
}

namespace std
{
  inline ostream &operator<<(ostream &o, kyaml::node const &n)
  {
    n.print(o);
    return o;
  }

  ostream &operator<<(ostream &o, std::shared_ptr<const kyaml::node> sp);

  inline ostream &operator<<(ostream &o, std::shared_ptr<kyaml::node> sp)
  {
    return o << (std::shared_ptr<const kyaml::node>(sp));
  }
}

#endif // NODE_HH

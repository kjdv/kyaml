#ifndef NODE_HH
#define NODE_HH

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <cassert>
#include <sstream>
#include <set>

namespace kyaml
{
  class node_visitor;

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

    typedef std::set<std::string> properties_t;

    virtual ~node()
    {}

    virtual type_t type() const = 0;

    // easy access: get as value, item from sequence, or value from map. may throw
    virtual std::string const &get() const; // throws if type() != SCALAR

    virtual node const &get(size_t i) const; // throws if type() != SEQUENCE

    virtual node const &get(std::string const &key) const; // throws if type() != MAPPING

    void add(std::shared_ptr<node> val);

    void add(std::string const &key, std::shared_ptr<node> val);

    // access child documents. Use integer-types to access items in sequence, strings as keys in mappings
    // will throw on type mismatch.

    node const &value() const
    {
      return *this;
    }

    template <typename head_t, typename... tail_t>
    node const &value(head_t const &head, tail_t... tail) const
    {
      return get(head).value(tail...);
    }

    // check membership of the item
    bool has() const // just the sentinel
    {
      return true;
    }

    template <typename... path_t>
    bool has(size_t idx, path_t... path) const
    {
      return
          type() == SEQUENCE &&
          get(idx).has(path...);
    }

    template <typename... path_t>
    bool has(std::string const &key, path_t... path) const
    {
      return
          type() == MAPPING &&
          get(key).has(path...);
    }

    // specialized value()/has() for leaf nodes
    template <typename... path_t>
    std::string const &leaf_value(path_t... path) const
    {
      return value(path...).get();
    }

    // sentinel
    bool has_leaf() const
    {
      return type() == SCALAR;
    }

    template <typename... path_t>
    bool has_leaf(size_t idx, path_t... path) const
    {
      return
          type() == SEQUENCE &&
          get(idx).has_leaf(path...);
    }

    template <typename... path_t>
    bool has_leaf(std::string const &key, path_t... path) const
    {
      return
          type() == MAPPING &&
          get(key).has_leaf(path...);
    }

    // node properties
    properties_t const &properties() const
    {
      return d_properties;
    }

    bool has_property(std::string const &prop) const
    {
      return d_properties.find(prop) != d_properties.end();
    }

    void add_property(std::string const &prop)
    {
      d_properties.insert(prop);
    }

    // relies on dynamic_cast, should be needed to much, throws on type mismatch
    scalar const &as_scalar() const;
    sequence const &as_sequence() const;
    mapping const &as_mapping() const;

    // visitor pattern
    virtual void accept(node_visitor &visitor) const = 0;

  private:
    properties_t d_properties;
  };

  typedef node document;

  class scalar final : public node
  {
  public: 
    scalar(std::string const &v) :
      d_value(v)
    {}

    type_t type() const override
    {
      return SCALAR;
    }

    std::string const &get() const override
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

    void accept(node_visitor &visitor) const override;

  private:
    std::string d_value;
  };

  class sequence final : public node
  {
  public:
    typedef std::vector<std::shared_ptr<const node> > container_t;

    type_t type() const override
    {
      return SEQUENCE;
    }

    node const &operator[](size_t i) const
    {
      return get(i);
    }

    node const &get(size_t i) const override
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

    void accept(node_visitor &visitor) const override;

  private:
    container_t d_items;
  };

  class mapping final : public node
  {
  public:
    typedef std::unordered_map<std::string, std::shared_ptr<const node> > container_t;

    type_t type() const override
    {
      return MAPPING;
    }

    node const &operator[](std::string const &key) const
    {
      return get(key);
    }

    node const &get(std::string const &key) const override;

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

    void accept(node_visitor &visitor) const override;

  private:
    container_t d_items;
  };
}

namespace std
{
  // useful for testing/debugging/diagnostic purposes
  // note: only use as such, the output format is not guaranteed to follow a certain pattern,
  // specifically, it is not guaranteed to be valid yaml/json formatting
  ostream &operator<<(ostream &o, kyaml::node const &n);
}

#endif // NODE_HH

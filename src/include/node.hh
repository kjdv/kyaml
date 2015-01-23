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
  // binary data will be represented by a vector of bytes
  typedef std::vector<uint8_t> binary_t;

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

    class value_error : public std::runtime_error
    {
    public:
      using std::runtime_error::runtime_error;
    };

    class type_error : public std::runtime_error
    {
    public:
      using std::runtime_error::runtime_error;
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
    node const &value(head_t const &head, tail_t&&... tail) const
    {
      return get(head).value(std::forward<tail_t>(tail)...);
    }

    // check membership of the item
    bool has() const // just the sentinel
    {
      return true;
    }

    template <typename... path_t>
    bool has(size_t idx, path_t&&... path) const
    {
      return
        type() == SEQUENCE &&
        has_index(idx) &&
        get(idx).has(std::forward<path_t>(path)...);
    }

    template <typename... path_t>
    bool has(std::string const &key, path_t&&... path) const
    {
      return
        type() == MAPPING &&
        has_key(key) &&
        get(key).has(std::forward<path_t>(path)...);
    }

    // specialized value()/has() for leaf nodes
    template <typename... path_t>
    std::string const &leaf_value(path_t&&... path) const
    {
      return value(std::forward<path_t>(path)...).get();
    }

    // sentinel
    bool has_leaf() const
    {
      return type() == SCALAR;
    }

    template <typename... path_t>
    bool has_leaf(size_t idx, path_t&&... path) const
    {
      return
        type() == SEQUENCE &&
        has_index(idx) &&
        get(idx).has_leaf(std::forward<path_t>(path)...);
    }

    template <typename... path_t>
    bool has_leaf(std::string const &key, path_t&&... path) const
    {
      return
        type() == MAPPING &&
        has_key(key) &&
        get(key).has_leaf(std::forward<path_t>(path)...);
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

  protected:
    virtual bool has_index(size_t idx) const
    {
      return false;
    }

    virtual bool has_key(std::string const &key) const
    {
      return false;
    }

  private:
    properties_t d_properties;
  };

  typedef node document;

  // type conversion for scalars.
  // by default, std::operator<< is used
  // specialized overloads are provided for bool and std::vector<uint8_t> (base64 binary)
  // you can also define your own.
  template <typename target_t>
  target_t type_convert(node::properties_t const &props, std::string const &input);

  class scalar final : public node
  {
  public: 
    // known values, currently don't really have a meaning
    static const std::string null_property;   // !!null
    static const std::string bool_property;   // !!bool
    static const std::string int_property;    // !!int
    static const std::string float_property;  // !!float
    static const std::string string_property; // !!str
    static const std::string binary_property; // !!binary

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

    // access the value as a specific type
    template <typename target_t>
    target_t as() const
    {
      return type_convert<target_t>(properties(), d_value);
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
      assert(i < size() && d_items[i]);
      return *d_items[i];
    }

    node const &get(size_t i) const override;

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

  protected:
    bool has_index(size_t idx) const override
    {
      return idx < size();
    }

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

    node const &operator[](std::string const &key) const;

    node const &get(std::string const &key) const override;

    bool has_key(std::string const &key) const override
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

  template <typename target_t>
  target_t type_convert(node::properties_t const &props, std::string const &input)
  {
    target_t result = target_t();
    std::stringstream str(input);
    str >> result;
    return result;
  }

  template<> // overload for bool
  bool type_convert(node::properties_t const &props, std::string const &input);

  template<> // overload for string
  inline std::string type_convert(node::properties_t const &props, std::string const &input)
  {
    return input;
  }

  template<> // overload for (base64) binary data
  binary_t type_convert(node::properties_t const &props, std::string const &input);
}

namespace std
{
  // useful for testing/debugging/diagnostic purposes
  // note: only use as such, the output format is not guaranteed to follow a certain pattern,
  // specifically, it is not guaranteed to be valid yaml/json formatting
  ostream &operator<<(ostream &o, kyaml::node const &n);
}

#endif // NODE_HH

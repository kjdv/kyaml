#include "pykyaml.hh"
#include <node_visitor.hh>

#include <stack>
#include <memory>

using namespace std;
using namespace kyaml;
using namespace pykyaml;

namespace
{
  class inserter
  {
  public:
    inserter(py_object container) :
      d_self(container)
    {}

    virtual void insert(py_object item) = 0;
    virtual void set_key(py_object key)
    {
      assert(false);
    }

    py_object collect()
    {
      return d_self;
    }

  protected:
    PyObject *get()
    {
      return d_self.get();
    }

  private:
    py_object d_self;
  };

  class list_inserter : public inserter
  {
  public:
    list_inserter() :
      inserter(py_object(PyList_New(0), false))
    {}

    void insert(py_object item) override
    {
      PyList_Append(get(), item.get());
    }
  };

  class dict_inserter : public inserter
  {
  public:
    dict_inserter() :
      inserter(py_object(PyDict_New(), false))
    {}

    void insert(py_object item) override
    {
      assert(d_key);

      PyDict_SetItem(get(), d_key->get(), item.get());
      d_key.reset();
    }

    void set_key(py_object key) override
    {
      assert(!d_key);
      d_key.reset(new py_object(key));
    }

  private:
    unique_ptr<py_object> d_key;
  };

  class leaf_inserter : public inserter
  {
  public:
    leaf_inserter(py_object leaf) :
      inserter(leaf)
    {}

    void insert(py_object item) override
    {
      assert(false);
    }
  };

  class py_visitor : public node_visitor
  {
  public:
    py_visitor(bool convert) :
      d_convert(convert)
    {}

    void visit(scalar const &val) override
    {
      if(d_convert)
      {
        if(val.has_property(scalar::bool_property))
          insert_bool(val);
        else if(val.has_property(scalar::int_property))
          insert_int(val);
        else if(val.has_property(scalar::float_property))
          insert_float(val);
        else if(val.has_property(scalar::string_property))
          insert_string(val);
        else if(val.has_property(scalar::null_property))
          insert_null(val);
        else if(val.has_property(scalar::binary_property))
          insert_binary(val);
        else
          insert_leaf(val);
      }
      else
        insert_leaf(val);
    }

    void visit(sequence const &seq) override
    {
      d_stack.emplace(new list_inserter);
    }

    void visit(mapping const &map) override
    {
      d_stack.emplace(new dict_inserter);
    }

    void sentinel(sequence const &seq) override
    {
      unwind();
    }

    void sentinel(mapping const &map) override
    {
      unwind();
    }

    void visit_key(std::string const &key) override
    {
      assert(!d_stack.empty());

      PyObject *pkey = PyString_FromStringAndSize(key.data(), key.size());
      d_stack.top()->set_key(py_object(pkey, false));
    }

    py_object collect()
    {
      assert(d_stack.size() == 1);
      return d_stack.top()->collect();
    }

  private:
    void insert_bool(scalar const &val)
    {
      bool v = val.as<bool>();
      py_object leaf(PyBool_FromLong(v), false);
      insert_base(leaf);
    }

    void insert_int(scalar const &val)
    {
      long v = val.as<long>();
      py_object leaf(PyInt_FromLong(v), false);
      insert_base(leaf);
    }

    void insert_float(scalar const &val)
    {
      double v = val.as<double>();
      py_object leaf(PyFloat_FromDouble(v), false);
      insert_base(leaf);
    }

    void insert_string(scalar const &val)
    {
      string  v = val.as<string>();
      py_object leaf(PyString_FromStringAndSize(v.data(), v.size()), false);
      insert_base(leaf);
    }

    void insert_null(scalar const &val)
    {
      py_object leaf(Py_None, true);
      insert_base(leaf);
    }

    void insert_binary(scalar const &val)
    {
      binary_t v = val.as<vector<uint8_t> >();
      py_object leaf(PyByteArray_FromStringAndSize((char const *)v.data(), v.size()), false);
      insert_base(leaf);
    }

    void insert_leaf(scalar const &val)
    {
      // todo: can we get rid of this extra copy?
      shared_ptr<scalar> cp = make_shared<scalar>(val.get());
      for(auto const &prop : val.properties())
        cp->add_property(prop);

      py_object leaf = build_leaf(cp);
      insert_base(leaf);
    }

    void insert_base(py_object leaf)
    {
      if(d_stack.empty())
        d_stack.emplace(new leaf_inserter(leaf));
      else
        d_stack.top()->insert(leaf);
    }

    void unwind()
    {
      assert(!d_stack.empty());

      unique_ptr<inserter> item(d_stack.top().release());
      d_stack.pop();

      if(d_stack.empty())
        d_stack.push(std::move(item));
      else
        d_stack.top()->insert(item->collect());
    }

    bool d_convert;
    stack<unique_ptr<inserter> > d_stack;
  };
}

namespace pykyaml
{
  py_object build_tree(document const &root, bool convert)
  {
    py_visitor visitor(convert);
    root.accept(visitor);

    return visitor.collect();
  }
}

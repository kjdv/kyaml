#include "pykyaml.hh"
#include "utils.hh"
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
    inserter(PyObject *container) :
      d_self(container, false)
    {}

    virtual void insert(PyObject *item) = 0;
    virtual void set_key(PyObject *key)
    {
      assert(false);
    }

    PyObject *collect()
    {
      PyObject *self = d_self.get();
      Py_INCREF(self);
      return self;
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
      inserter(PyList_New(0))
    {}

    void insert(PyObject *item) override
    {
      PyList_Append(get(), item);
      Py_DECREF(item);
    }
  };

  class dict_inserter : public inserter
  {
  public:
    dict_inserter() :
      inserter(PyDict_New()),
      d_key(nullptr)
    {}

    void insert(PyObject *item) override
    {
      assert(d_key);
      PyDict_SetItem(get(), d_key, item);
      Py_DECREF(d_key);
      d_key = nullptr;
    }

    void set_key(PyObject *key) override
    {
      assert(!d_key);
      d_key = key;
    }

  private:
    PyObject *d_key;
  };

  class leaf_inserter : public inserter
  {
  public:
    leaf_inserter(PyObject *leaf) :
      inserter(leaf)
    {}

    void insert(PyObject *item) override
    {
      assert(false);
    }
  };

  class py_visitor : public node_visitor
  {
  public:
    void visit(scalar const &val) override
    {
      // todo: can we get rid of this extra copy?
      shared_ptr<scalar> cp = make_shared<scalar>(val.get());
      for(auto const &prop : val.properties())
        cp->add_property(prop);

      PyObject *leaf = build_leaf(cp);
      if(d_stack.empty())
        d_stack.emplace(new leaf_inserter(leaf));
      else
        d_stack.top()->insert(leaf);
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
      d_stack.top()->set_key(pkey);
    }

    PyObject *collect()
    {
      assert(d_stack.size() == 1);
      return d_stack.top()->collect();
    }

  private:
    void unwind()
    {
      if(d_stack.size() > 1)
      {
        shared_ptr<inserter> item = d_stack.top(); d_stack.pop();
        d_stack.top()->insert(item->collect());
      }
    }

    stack<shared_ptr<inserter> > d_stack;
  };
}

namespace pykyaml
{
  PyObject *build_tree(document const &root)
  {
    py_visitor visitor;
    root.accept(visitor);

    return visitor.collect();
  }
}

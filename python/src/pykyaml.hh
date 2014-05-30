#ifndef PYKYAML_HH
#define PYKYAML_HH

#include <Python.h>
#include <kyaml.hh>
#include <stdexcept>

namespace pykyaml
{
  // wrapper object for PyObject*
  class py_object
  {
  public:
    py_object(PyObject *self, bool incref = true) :
      d_self(self)
    {
      if(incref)
        Py_INCREF(d_self);
    }

    py_object(py_object const &other)
    {
      copy(other);
    }

    ~py_object()
    {
      destroy();
    }

    py_object &operator=(py_object const &other)
    {
      if(this != &other)
      {
        destroy();
        copy(other);
      }
      return *this;
    }

    PyObject const *get() const
    {
      return d_self;
    }

    PyObject *get()
    {
      return d_self;
    }

  private:
    void copy(py_object const &other)
    {
      d_self = other.d_self;
      Py_INCREF(d_self);
    }

    void destroy()
    {
      Py_DECREF(d_self);
    }

    PyObject *d_self;
  };

  class type_error : public std::exception
  {
  public:
    type_error(std::string const &msg) :
      d_msg(msg)
    {}

    char const *what() const throw() override
    {
      return d_msg.c_str();
    }

  private:
    std::string d_msg;
  };

  class value_error : public std::exception
  {
  public:
    value_error(std::string const &msg) :
      d_msg(msg)
    {}

    char const *what() const throw() override
    {
      return d_msg.c_str();
    }

  private:
    std::string d_msg;
  };

  class call_checker
  {
  public:
    static void initialize(std::string const &name)
    {
      s_exception = PyErr_NewException((char *)name.c_str(), nullptr, nullptr);
      Py_INCREF(s_exception);
    }

    template <typename callable_t, typename... args_t>
    PyObject *call(callable_t &callable, args_t... args)
    {
      assert(s_exception);

      try
      {
        return callable(args...);
      }
      catch(std::exception const &e)
      {
        PyErr_SetString(s_exception, e.what());
        return nullptr;
      }
    }

    static PyObject *get()
    {
      return s_exception;
    }

  private:
    static PyObject *s_exception;
  };

  PyObject *build_leaf(std::shared_ptr<const kyaml::scalar> value);

  void init_parser(PyObject *module);
  void init_leaf(PyObject *module);
}

#endif // PYKYAML_HH

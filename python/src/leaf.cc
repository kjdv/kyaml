#include "pykyaml.hh"

using namespace std;
using namespace kyaml;
using namespace pykyaml;

namespace
{
  // structure ctor/dtor

  struct leaf_t
  {
    PyObject_HEAD

    shared_ptr<const scalar> value;
  };

  PyObject *allocate(PyTypeObject *type, PyObject *args, PyObject *kwds)
  {
    leaf_t *self = (leaf_t *)type->tp_alloc(type, 0);
    self->value.reset();

    return (PyObject *)self;
  }

  void destroy(leaf_t *self)
  {
    assert(self);

    self->value.reset();
    self->ob_type->tp_free((PyObject *)self);
  }

  // methods
  PyObject *value(leaf_t *self, PyObject *arg)
  {
    assert(self);
    assert(self->value);

    string const &str = self->value->get();
    return PyString_FromStringAndSize(str.c_str(), str.size());
  }

  PyObject *properties(leaf_t *self, PyObject *arg)
  {
    assert(self);
    assert(self->value);

    if(!self->value->properties().empty())
    {
      PyObject *set = PySet_New(nullptr);
      for(string const &item : self->value->properties())
      {
        py_object key(PyString_FromStringAndSize(item.c_str(), item.size()), false);
        PySet_Add(set, key.get());
      }
      return set;
    }
    Py_RETURN_NONE;
  }

  py_object as_bool_delegate(scalar const &val)
  {
    bool v = val.as<bool>();
    return py_object(PyBool_FromLong(v), false);
  }

  PyObject *as_bool(leaf_t *self, PyObject *arg)
  {
    assert(self);
    assert(self->value);

    return call_checker().call(as_bool_delegate, *self->value).release();
  }

  py_object as_int_delegate(scalar const &val)
  {
    long v = val.as<long>();
    return py_object(PyInt_FromLong(v), false);
  }

  PyObject *as_int(leaf_t *self, PyObject *arg)
  {
    assert(self);
    assert(self->value);

    return call_checker().call(as_int_delegate, *self->value).release();
  }

  py_object as_float_delegate(scalar const &val)
  {
    double v = val.as<double>();
    return py_object(PyFloat_FromDouble(v), false);
  }

  PyObject *as_float(leaf_t *self, PyObject *arg)
  {
    assert(self);
    assert(self->value);

    return call_checker().call(as_float_delegate, *self->value).release();
  }

  py_object as_string_delegate(scalar const &val)
  {
    string const &str = val.get();
    return py_object(PyString_FromStringAndSize(str.c_str(), str.size()), false);
  }

  PyObject *as_string(leaf_t *self, PyObject *arg)
  {
    assert(self);
    assert(self->value);

    return call_checker().call(as_string_delegate, *self->value).release();
  }

  py_object as_binary_delegate(scalar const &val)
  {
    std::vector<uint8_t> v = val.as<std::vector<uint8_t> >();
    return py_object(PyByteArray_FromStringAndSize((char const *)v.data(), v.size()), false);
  }

  PyObject *as_binary(leaf_t *self, PyObject *arg)
  {
    assert(self);
    assert(self->value);

    return call_checker().call(as_binary_delegate, *self->value).release();
  }

  PyObject *representation(PyObject *self)
  {
    assert(self);
    leaf_t *as_leaf = (leaf_t *)self;
    return value(as_leaf, nullptr);
  }

  // python magic

  PyMethodDef leaf_methods[] =
  {
    {"value",      (PyCFunction)value,      METH_NOARGS, ""},
    {"properties", (PyCFunction)properties, METH_NOARGS, ""},

    {"as_bool",    (PyCFunction)as_bool,    METH_NOARGS, ""},
    {"as_int",     (PyCFunction)as_int,     METH_NOARGS, ""},
    {"as_float",   (PyCFunction)as_float,   METH_NOARGS, ""},
    {"as_string",  (PyCFunction)as_string,  METH_NOARGS, ""},
    {"as_binary",  (PyCFunction)as_binary,  METH_NOARGS, ""},

    {nullptr, nullptr, 0, nullptr}
  };

  PyTypeObject leaf_type =
  {
    PyObject_HEAD_INIT(nullptr)
    0,                                        // ob_size
    "kyaml.leaf",                             // tp_name
    sizeof(leaf_t),                           // tp_basicsize
    0,                                        // tp_itemsize
    (destructor)destroy,                      // tp_dealloc
    0,                                        // tp_print
    0,                                        // tp_getattr
    0,                                        // tp_setattr
    0,                                        // tp_compare
    representation,                           // tp_repr
    0,                                        // tp_as_number
    0,                                        // tp_as_sequence
    0,                                        // tp_as_mapping
    0,                                        // tp_hash
    0,                                        // tp_call
    representation,                           // tp_str
    0,                                        // tp_getattro
    0,                                        // tp_setattro
    0,                                        // tp_as_buffer
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, // tp_flags
    "leaf",                                   // tp_doc
    0,                                        // tp_traverse
    0,                                        // tp_clear
    0,                                        // tp_richcompare
    0,                                        // tp_weaklistoffset
    0,                                        // tp_iter
    0,                                        // tp_iternext
    leaf_methods,                             // tp_methods
    0,                                        // tp_members
    0,                                        // tp_getseters
    0,                                        // tp_base
    0,                                        // tp_dict
    0,                                        // tp_descr_get
    0,                                        // tp_descr_set
    0,                                        // tp_dictoffset
    0, // (initproc)initialize,               // tp_init // lets prohibit construction by the user
    0,                                        // tp_alloc
    0, // allocate,                           // tp_new  // lets prohibit construction by the user
  };
}

namespace pykyaml
{

  py_object build_leaf(shared_ptr<const scalar> value)
  {
    leaf_t *self = (leaf_t *)allocate(&leaf_type, nullptr, nullptr);
    self->value = value;
    return py_object((PyObject *)self, false);
  }

  void init_leaf(PyObject *module)
  {
    if(PyType_Ready(&leaf_type) < 0)
      return;

    Py_INCREF(&leaf_type);
    PyModule_AddObject(module, "leaf", (PyObject *)&leaf_type);
  }
}

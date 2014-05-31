#include "pykyaml.hh"
#include "pystream.hh"

using namespace pykyaml;

namespace
{
  // structure ctor/dtor

  struct parser_t
  {
    PyObject_HEAD

    py_istream *stream;
    kyaml::parser *parser;
  };

  PyObject *allocate(PyTypeObject *type, PyObject *args, PyObject *kwds)
  {
    parser_t *self = (parser_t *)type->tp_alloc(type, 0);
    self->parser = nullptr;
    self->stream = nullptr;

    return (PyObject *)self;
  }

  int initialize(parser_t *self, PyObject *args, PyObject *kwds)
  {
    PyObject *file;

    static char *kwlist[] = {(char *)"stream", nullptr};

    if(!PyArg_ParseTupleAndKeywords(args, kwds, "O", kwlist, &file))
      return -1;

    py_object file_object(file);
    if(!py_istream::check(file_object))
    {
      PyErr_BadArgument();
      return -1;
    }

    self->stream = new py_istream(file_object);
    self->parser = new kyaml::parser(*self->stream);

    return 0;
  }

  void destroy(parser_t *self)
  {
    delete self->stream;
    delete self->parser;

    self->ob_type->tp_free((PyObject *)self);
  }

  // methods
  PyObject *parse_delegate(parser_t *self)
  {
    assert(self);

    std::unique_ptr<const kyaml::document> root = self->parser->parse();
    if(root)
      return build_tree(*root);

    Py_RETURN_NONE;
  }

  PyObject *parse(parser_t *self, PyObject *arg)
  {
    return call_checker().call(parse_delegate, self);
  }

  PyObject *peek(parser_t *self, PyObject *arg)
  {
    long n = PyInt_AsLong(arg);
    if(n < 0)
    {
      PyErr_BadArgument();
      return nullptr;
    }

    std::string peeked = self->parser->peek(n);
    PyObject *result = PyString_FromStringAndSize(peeked.data(), peeked.size());
    return result;
  }

  PyObject *linenumber(parser_t *self, PyObject *arg)
  {
    assert(self);
    return PyInt_FromLong(self->parser->linenumber());
  }

  // python magic

  PyMethodDef parser_methods[] =
  {
    {"parse",      (PyCFunction)parse,      METH_NOARGS, ""},
    {"peek",       (PyCFunction)peek,       METH_O,      ""},
    {"linenumber", (PyCFunction)linenumber, METH_NOARGS, ""},

    {nullptr, nullptr, 0, nullptr}
  };

  PyTypeObject parser_type =
  {
    PyObject_HEAD_INIT(nullptr)
    0,                                        // ob_size
    "kyaml.parser",                           // tp_name
    sizeof(parser_t),                         // tp_basicsize
    0,                                        // tp_itemsize
    (destructor)destroy,                      // tp_dealloc
    0,                                        // tp_print
    0,                                        // tp_getattr
    0,                                        // tp_setattr
    0,                                        // tp_compare
    0,                                        // tp_repr
    0,                                        // tp_as_number
    0,                                        // tp_as_sequence
    0,                                        // tp_as_mapping
    0,                                        // tp_hash
    0,                                        // tp_call
    0,                                        // tp_str
    0,                                        // tp_getattro
    0,                                        // tp_setattro
    0,                                        // tp_as_buffer
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, // tp_flags
    "parser",                                 // tp_doc
    0,                                        // tp_traverse
    0,                                        // tp_clear
    0,                                        // tp_richcompare
    0,                                        // tp_weaklistoffset
    0,                                        // tp_iter
    0,                                        // tp_iternext
    parser_methods,                           // tp_methods
    0,                                        // tp_members
    0,                                        // tp_getseters
    0,                                        // tp_base
    0,                                        // tp_dict
    0,                                        // tp_descr_get
    0,                                        // tp_descr_set
    0,                                        // tp_dictoffset
    (initproc)initialize,                     // tp_init
    0,                                        // tp_alloc
    allocate,                                 // tp_new
  };
}

namespace pykyaml
{
  void init_parser(PyObject *module)
  {
    if(PyType_Ready(&parser_type) < 0)
      return;

    Py_INCREF(&parser_type);
    PyModule_AddObject(module, "parser", (PyObject *)&parser_type);
  }
}

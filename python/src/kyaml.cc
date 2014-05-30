#include <Python.h>
#include "kyaml.hh"

namespace
{
  PyObject *dummy(PyObject *self, PyObject *arg)
  {
    return NULL;
  }

  PyMethodDef module_methods[] =
  {
    {"dummy", (PyCFunction)dummy, METH_NOARGS, ""},
    {NULL, NULL, 0, NULL}
  };
}

PyMODINIT_FUNC initpykyaml(void)
{
  PyObject *module = Py_InitModule("pykyaml", module_methods);

  if(!module)
    return;

  PyModule_AddIntConstant(module, "ONE", 1);

}

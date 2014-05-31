#include "pykyaml.hh"
#include "pystream.hh"

using namespace std;
using namespace pykyaml;

PyObject *call_checker::s_exception = nullptr;

namespace
{
  PyMethodDef module_methods[] =
  {
    {nullptr, nullptr, 0, nullptr}
  };
}

PyMODINIT_FUNC initpykyaml(void)
{
  PyObject *module = Py_InitModule("pykyaml", module_methods);

  if(!module)
    return;

  call_checker::initialize("kyaml.error");
  PyModule_AddObject(module, "error", call_checker::get());

  init_parser(module);
  init_leaf(module);
}

#include "pykyaml.hh"
#include "pystream.hh"

using namespace std;
using namespace kyaml;
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

  PyModule_AddStringConstant(module, "null_property", scalar::null_property.c_str());
  PyModule_AddStringConstant(module, "bool_property", scalar::bool_property.c_str());
  PyModule_AddStringConstant(module, "int_property", scalar::int_property.c_str());
  PyModule_AddStringConstant(module, "float_property", scalar::float_property.c_str());
  PyModule_AddStringConstant(module, "string_property", scalar::string_property.c_str());
  PyModule_AddStringConstant(module, "binary_property", scalar::binary_property.c_str());
}

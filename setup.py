from distutils.core import setup, Extension
import os

def globcc(dir, extention = '.cc'):
    result = []
    for file in os.listdir(dir):
        abspath = os.path.join(dir, file)
        if os.path.isfile(abspath) and abspath.endswith(extention):
            result.append(abspath)
    return result

def sourcedir():
    return os.path.dirname(__file__)

def targetdir():
    return os.getcwd()

sources = globcc(os.path.join(sourcedir(), 'python', 'src'))
includes = [os.path.join(sourcedir(), 'src', 'include')]
libraries = ['kyaml']
library_dirs = [targetdir()]

pykyaml = Extension('pykyaml',
                    sources = sources,
                    include_dirs = includes,
                    libraries = libraries,
                    library_dirs = library_dirs)

# todo: a more portable way would be nice
pykyaml.extra_compile_args = ['-std=c++11']

setup(name = 'pykyaml', version = '0.1', description = 'Klaas\' implementation for yaml', ext_modules = [pykyaml])

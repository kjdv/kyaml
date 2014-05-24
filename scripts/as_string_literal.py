#! /usr/bin/python

import sys

def indent(indent_level, output):
    output.write(' ' * indent_level)

def as_string_literal(varname, indent_level, input, output):
    indent(indent_level, output)
    output.write('const std::string %s =' % varname)
                 
    for line in input:
        escaped = line.replace('"', '\\"').rstrip('\n')
        output.write('\n')
        indent(indent_level + 2, output)
        output.write('"%s\\n"' % escaped)

    output.write(';\n')

if __name__ == '__main__':
    varname = sys.argv[1] if len(sys.argv) > 1 else 'as_string'
    indent_level = int(sys.argv[2]) if len(sys.argv) > 2 else 0
    as_string_literal(varname, indent_level, sys.stdin, sys.stdout)

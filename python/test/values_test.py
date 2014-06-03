import unittest
import pykyaml as kyaml

import tempfile

class values_test(unittest.TestCase):
    doc = '''---
null : 
binary : !unspecified c29tZSBiaW5hcnk=
bool_yes : Yes
bool_no : No
integer : -5
float : 2.71828
string : "blah	blah\\tblah"
explicit:
  bool : !!bool true
  integer : !!int 10
  float : !!float 0.123
  string : !!str "foo foo"
  null : !!null
  binary : !!binary c29tZSBiaW5hcnk=
'''
    def setUp(self):
        file = tempfile.TemporaryFile('w+b')

        file.write(values_test.doc)
        file.seek(0)

        parser = kyaml.parser(file)
        self.root = parser.parse()

    def test_property(self):
        self.assertEqual(set(['!unspecified']), self.root['binary'].properties())

    def test_binary(self):
        self.assertEqual('some binary', self.root['binary'].as_binary())

    def test_yes(self):
        self.assertTrue(self.root['bool_yes'].as_bool())

    def test_no(self):
        self.assertFalse(self.root['bool_no'].as_bool())

    def test_integer(self):
        self.assertEquals(-5, self.root['integer'].as_int())

    def test_float(self):
        self.assertEquals(2.71828, self.root['float'].as_float())

    def test_string(self):
        self.assertEquals('blah\tblah\tblah', self.root['string'].as_string())

    def test_null(self):
        self.assertEquals(None, self.root['null'].as_null())
        
    def test_explicit_bool(self):
        val = self.root['explicit']['bool']
        self.assertEquals(bool, type(val))
        self.assertEquals(True, val)

    def test_explicit_int(self):
        val = self.root['explicit']['integer']
        self.assertEquals(int, type(val))
        self.assertEquals(10, val)

    def test_explicit_float(self):
        val = self.root['explicit']['float']
        self.assertEquals(float, type(val))
        self.assertEquals(0.123, val)

    def test_explicit_string(self):
        val = self.root['explicit']['string']
        self.assertEquals(str, type(val))
        self.assertEquals('foo foo', val)

    def test_explicit_null(self):
        val = self.root['explicit']['null']
        self.assertEquals(type(None), type(val))
        self.assertEquals(None, val)

    def test_explicit_binary(self):
        val = self.root['explicit']['binary']
        self.assertEquals(bytearray, type(val))
        self.assertEquals('some binary', val)

    def test_inhibit_implicit_type_conversion(self):
        # can we surpress the implicit type conversion
        # when we want or have to?

        file = tempfile.TemporaryFile('w+b')
        file.write(values_test.doc)
        file.seek(0)

        parser = kyaml.parser(file)
        root = parser.parse(False)       
        val = root['explicit']['float']

        self.assertEquals(kyaml.leaf, type(val))
        self.assertEquals('0.123', val.value())

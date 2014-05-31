import unittest
import pykyaml as kyaml

import tempfile

class parser_test(unittest.TestCase):
    doc = '''---
receipt:     Oz-Ware Purchase Invoice
date:        2012-08-06
customer:
    given:   Dorothy
    family:  Gale

items:
    - part_no:   A4786
      descrip:   Water Bucket (Filled)
      price:     1.47
      quantity:  4

    - part_no:   E1628
      descrip:   High Heeled "Ruby" Slippers
      size:      8
      price:     100.27
      quantity:  1

bill-to:  &id001
    street: |
            123 Tornado Alley
            Suite 16
    city:   East Centerville
    state:  FL

ship-to:  *id001

specialDelivery:  >
    Follow the Yellow Brick
    Road to the Emerald City.
    Pay no attention to the
    man behind the curtain.
...
---
# next doc as sentinel
binary : !!binary c29tZSBiaW5hcnk=
bool_yes : !!bool Yes
bool_no : !!bool No
integer : !!int -5
float : !!float 2.71828
string : !!str "blah	blah\\tblah"
null : !!null
'''

    def setUp(self):
        file = tempfile.TemporaryFile('rw+b')

        file.write(parser_test.doc)
        file.seek(0)

        self.parser = kyaml.parser(file)

    def test_parse(self):
        self.parser.parse()
        self.assertEqual(35, self.parser.linenumber())

    def test_peek(self):
        self.parser.parse()
        sentinel = '---\n# next doc as sentinel\n'
        self.assertEqual(sentinel, self.parser.peek(len(sentinel)))

    def test_value(self):
        root = self.parser.parse()
        self.assertEqual('E1628', root['items'][1]['part_no'].value())

    def test_conversion(self):
        root = self.parser.parse()
        self.assertEqual(100.27, root['items'][1]['price'].as_float())

    def test_anchor(self):
        root = self.parser.parse()
        self.assertEqual('East Centerville', root['ship-to']['city'].value())

    def test_property(self):
        self.parser.parse() # skip first document
        root = self.parser.parse()
        self.assertEqual(set(['!!binary']), root['binary'].properties())

    def test_binary(self):
        self.parser.parse() # skip first document
        root = self.parser.parse()
        self.assertEqual('some binary', root['binary'].as_binary())

    def test_yes(self):
        self.parser.parse() # skip first document
        root = self.parser.parse()
        self.assertTrue(root['bool_yes'].as_bool())

    def test_no(self):
        self.parser.parse() # skip first document
        root = self.parser.parse()
        self.assertFalse(root['bool_no'].as_bool())

    def test_integer(self):
        self.parser.parse() # skip first document
        root = self.parser.parse()
        self.assertEquals(-5, root['integer'].as_int())

    def test_float(self):
        self.parser.parse() # skip first document
        root = self.parser.parse()
        self.assertEquals(2.71828, root['float'].as_float())

    def test_string(self):
        self.parser.parse() # skip first document
        root = self.parser.parse()
        self.assertEquals('blah\tblah\tblah', root['string'].as_string())

    def test_null(self):
        self.parser.parse() # skip first document
        root = self.parser.parse()
        self.assertEquals(None, root['null'].as_null())
        



        

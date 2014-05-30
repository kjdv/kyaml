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
        val = self.parser.parse()
        self.assertEqual('3.1459', val.value())
        self.assertEqual(frozenset(('aap', 'noot', 'mies')), val.properties())

    def test_conversion(self):
        val = self.parser.parse()
        self.assertEqual(3.1459, val.as_float())



        

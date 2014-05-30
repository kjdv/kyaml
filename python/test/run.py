#!/usr/bin/python

import unittest
import pykyaml as kyaml

# just a dumb simple check, mostly to test the build environment
# and module loading
class basic(unittest.TestCase):
    def test_basic(self):
        self.assertEqual(1, kyaml.ONE)

if __name__ == '__main__':
    unittest.main()

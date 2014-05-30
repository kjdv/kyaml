import unittest
import pykyaml as kyaml

import tempfile

class utils_test(unittest.TestCase):
    def setUp(self):
        self.file = tempfile.TemporaryFile('rw+b')

    def tearDown(self):
        del self.file

    def test_oneline(self):
        self.file.write('one line\n')
        self.file.seek(0)
        self.assertEqual('one line', kyaml.readline(self.file))

    def test_throw(self):
        self.assertRaises(kyaml.error, kyaml.throw)
 

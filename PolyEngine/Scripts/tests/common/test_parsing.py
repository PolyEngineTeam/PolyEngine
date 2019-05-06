import unittest

from commands.common.parsing import *

class Test(unittest.TestCase):
    def setUp(self):
        pass

    def tearDown(self):
        pass

    def test_first_tagged_line_from_text_simple(self):
        line = first_tagged_line_from_text('abcd\n@tag efgh\nijkm', '@tag')
        self.assertEqual(line, 'efgh')\

    def test_first_tagged_line_from_text_simple(self):
        line = first_tagged_line_from_text('abcd\n@tag efgh\nijkm', '@tag')
        self.assertEqual(line, 'efgh')
import unittest
import os
import json

import common.serialization
from common.version import Version

class Test(unittest.TestCase):
    def setUp(self):
        pass

    def tearDown(self):
        pass

    def test_valid_construction(self):
        ver = Version(1, 2, 3)
        self.assertEqual(ver.major, 1)
        self.assertEqual(ver.minor, 2)
        self.assertEqual(ver.fix, 3)

    def test_invalid_construction(self):
        with self.assertRaises(TypeError):
            Version(1.0, 2, 3)

        with self.assertRaises(TypeError):
            Version(1, 2.0, 3)

        with self.assertRaises(TypeError):
            Version(1, 2, 'a')

    def test_supports(self):
        ver = Version(5, 6, 7)
        ver_compat_fix1 = Version(5, 6, 1)
        ver_compat_fix2 = Version(5, 6, 9)
        ver_compat_min = Version(5, 4, 1)
        ver_compat_max = Version(5, 6, 7)

        ver_noncompat_min = Version(5, 8, 1)
        ver_noncompat_max1 = Version(3, 1, 1)
        ver_noncompat_max2 = Version(6, 1, 1)

        self.assertTrue(ver.supports(ver))

        self.assertTrue(ver.supports(ver_compat_fix1))
        self.assertTrue(ver.supports(ver_compat_fix2))

        self.assertTrue(ver.supports(ver_compat_min))
        self.assertTrue(ver.supports(ver_compat_max))

        self.assertFalse(ver.supports(ver_noncompat_min))
        self.assertFalse(ver.supports(ver_noncompat_max1))
        self.assertFalse(ver.supports(ver_noncompat_max2))

    def test_eq(self):
        ver1 = Version(1, 2, 3)
        ver2 = Version(1, 2, 3)
        ver3 = Version(4, 5, 6)

        self.assertEqual(ver1, ver2)
        self.assertEqual(ver2, ver1)

        self.assertNotEqual(ver1, ver3)
        self.assertNotEqual(ver3, ver1)

        self.assertNotEqual(ver2, ver3)
        self.assertNotEqual(ver3, ver2)

    def test_lt(self):
        ver = Version(5, 6, 7)
        ver_fix_l = Version(5, 6, 6)
        ver_fix_h = Version(5, 6, 8)
        ver_min_l = Version(5, 5, 9)
        ver_min_h = Version(5, 7, 1)
        ver_maj_l = Version(4, 9, 9)
        ver_maj_h = Version(6, 1, 1)

        self.assertLess(ver_fix_l, ver)
        self.assertLess(ver, ver_fix_h)

        self.assertLess(ver_min_l, ver)
        self.assertLess(ver, ver_min_h)

        self.assertLess(ver_maj_l, ver)
        self.assertLess(ver, ver_maj_h)

    def test_gt(self):
        ver = Version(5, 6, 7)
        ver_fix_l = Version(5, 6, 6)
        ver_fix_h = Version(5, 6, 8)
        ver_min_l = Version(5, 5, 9)
        ver_min_h = Version(5, 7, 1)
        ver_maj_l = Version(4, 9, 9)
        ver_maj_h = Version(6, 1, 1)

        self.assertGreater(ver_fix_h, ver)
        self.assertGreater(ver, ver_fix_l)

        self.assertGreater(ver_min_h, ver)
        self.assertGreater(ver, ver_min_l)

        self.assertGreater(ver_maj_h, ver)
        self.assertGreater(ver, ver_maj_l)

    def test_serialization(self):
        ver1 = Version(1, 2, 3)
        ver_str = json.dumps(ver1, cls=common.serialization.JSONEncoder)
        ver2 = json.loads(ver_str, cls=common.serialization.JSONDecoder)

        self.assertEqual(ver1, ver2)
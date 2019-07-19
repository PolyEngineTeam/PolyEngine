import numbers

import common.serialization

VERSION_MAJOR_NAME = 'Major'
VERSION_MINOR_NAME = 'Minor'
VERSION_FIX_NAME = 'Fix'

@common.serialization.serializable(
    lambda obj: { VERSION_MAJOR_NAME : obj.major, VERSION_MINOR_NAME : obj.minor, VERSION_FIX_NAME : obj.fix },
    lambda dct: Version(dct[VERSION_MAJOR_NAME], dct[VERSION_MINOR_NAME], dct[VERSION_FIX_NAME])
    )
class Version():
    def __init__(self, major, minor, fix):
        if not isinstance(major, numbers.Integral):
            raise TypeError('Major version must be and integral number. Actual type: {}'.format(type(major)))
        if not isinstance(minor, numbers.Integral):
            raise TypeError('Minor version must be and integral number. Actual type: {}'.format(type(minor)))
        if not isinstance(fix, numbers.Integral):
            raise TypeError('Fix version must be and integral number. Actual type: {}'.format(type(fix)))
        
        self.major = major
        self.minor = minor
        self.fix = fix

    def __repr__(self):
        return 'Version(major={}, minor={}, fix={})'.format(self.major, self.minor, self.fix)

    def __str__(self):
        return '{}.{}.{}'.format(self.major, self.minor, self.fix)

    def __eq__(self, other):
        if not isinstance(other, Version):
            raise TypeError('Cannot compare version with object of type: {}'.format(type(other)))
        return self.major == other.major and self.minor == other.minor and self.fix == other.fix

    def is_backwards_compatible_with(self, other):
        if not isinstance(other, Version):
            raise TypeError('Cannot check for backwards compatibility with object of type: {}'.format(type(other)))
        return self.major == other.major and self.minor <= other.minor
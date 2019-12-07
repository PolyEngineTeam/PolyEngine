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

    def supports(self, other):
        '''
        This method returns true when self is a newer version that is backwards compatible with other
        '''
        
        if not isinstance(other, Version):
            raise TypeError('Cannot check for backwards compatibility with object of type: {}'.format(type(other)))
        return self.major == other.major and self.minor >= other.minor

    def __repr__(self):
        return 'Version(major={}, minor={}, fix={})'.format(self.major, self.minor, self.fix)

    def __str__(self):
        return '{}.{}.{}'.format(self.major, self.minor, self.fix)

    def __eq__(self, other):
        if not isinstance(other, Version):
            raise TypeError('Cannot compare version with object of type: {}'.format(type(other)))
        return self.major == other.major and self.minor == other.minor and self.fix == other.fix

    def __lt__(self, other):
        if self.major < other.major:
            return True
        elif self.major == other.major:
            if self.minor < other.minor:
                return True
            elif self.minor == other.minor and self.fix < other.fix:
                return True
        return False

    def __gt__(self, other):
        if self.major > other.major:
            return True
        elif self.major == other.major:
            if self.minor > other.minor:
                return True
            elif self.minor == other.minor and self.fix > other.fix:
                return True
        return False

    def __le__(self, other):
        return self == other or self < other
    
    def __ge__(self, other):
        return self == other or self > other
        
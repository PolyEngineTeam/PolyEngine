import json
from functools import wraps

TYPE_KEYWORD = '@type'

_serializers = {}
_type_to_name = {}

def serializable(serializer, deserializer, name_override = None):
    def impl(cls):
        type_name = name_override if name_override else cls.__name__
        
        _type_to_name[cls] = type_name

        print(type_name)
        if type_name in _serializers:
            raise TypeError('Type of name [{}] has its serializers/deserialized already registered.')
        _serializers[type_name] = (serializer, deserializer)
        return cls
    return impl

class JSONEncoder(json.JSONEncoder):
    def default(self, obj):
        type_name = _type_to_name[type(obj)]
        if type_name in _serializers:
            dct = _serializers[type_name][0](obj)
            dct[TYPE_KEYWORD] = type_name
            return dct
        
        return json.JSONEncoder.default(self, obj)

class JSONDecoder(json.JSONDecoder):
    @wraps(json.JSONDecoder.__init__)
    def __init__(self, *args, **kwargs):
        OBJECT_HOOK_ARG_NAME = 'object_hook'
        self._ext_object_hook = None
        if OBJECT_HOOK_ARG_NAME in kwargs:
            self._ext_object_hook = kwargs[OBJECT_HOOK_ARG_NAME]
            del kwargs[OBJECT_HOOK_ARG_NAME]
        json.JSONDecoder.__init__(self, object_hook=self._object_hook, *args, **kwargs)

    def _object_hook(self, dct):
        if TYPE_KEYWORD in dct:
            type_name = dct[TYPE_KEYWORD]
            if type_name in _serializers:
                return _serializers[type_name][1](dct)

        if self._ext_object_hook:
            return self._ext_object_hook(dct)

        return dct
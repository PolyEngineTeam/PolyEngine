import json
from functools import wraps


_serializers = {}

def serializable(serializer, deserializer):
    def impl(cls):
        type_name = cls.__name__
        print(type_name)
        assert(type_name not in _serializers)
        _serializers[type_name] = (serializer, deserializer)
        return cls
    return impl

class JSONEncoder(json.JSONEncoder):
    def default(self, obj):
        type_name = type(obj).__name__
        if type_name in _serializers:
            dct = _serializers[type_name][0](obj)
            dct['@type'] = type_name
            return dct
        
        return json.JSONEncoder.default(self, obj)

class JSONDecoder(json.JSONDecoder):
    @wraps(json.JSONDecoder.__init__)
    def __init__(self, *args, **kwargs):
        self._ext_object_hook = None
        if 'object_hook' in kwargs:
            self._ext_object_hook = kwargs['object_hook']
            del kwargs['object_hook']
        json.JSONDecoder.__init__(self, object_hook=self._object_hook, *args, **kwargs)

    def _object_hook(self, dct):
        if '@type' in dct:
            type_name = dct['@type']
            if type_name in _serializers:
                return _serializers[type_name][1](dct)

        if self._ext_object_hook:
            return self._ext_object_hook(dct)

        return dct
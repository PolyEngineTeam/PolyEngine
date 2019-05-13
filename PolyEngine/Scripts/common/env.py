import os
import logging
import sys
import json

from collections import namedtuple

import common.serialization

# Parameters
_COMMANDS_DIR_NAME = 'commands'
_RESOURCES_DIR_NAME = 'resources'
_TESTS_DIR_NAME = 'tests'
_THIRDPARTY_DIR_NAME = 'ThirdParty'
_LOG_LEVEL = 'DEBUG'

_LOGGER_FORMAT = '[%(asctime)s] [%(levelname)s] [%(name)s]\t %(message)s'

# Constants
_FILE_DIR_PATH = os.path.dirname(os.path.realpath(__file__))
_SCRIPTS_ROOT = os.path.join(_FILE_DIR_PATH, os.pardir)

#EngineVersion = namedtuple('EngineVersion', ['major', 'minor', 'fix'])

def version_serializer(obj):
    return {
        'Major' : obj.major,
        'Minor' : obj.minor,
        'Fix' : obj.fix
    }

def version_deserializer(dct):
    return Version(dct['Major'], dct['Minor'], dct['Fix'])

@common.serialization.serializable(version_serializer, version_deserializer)
class Version():
    def __init__(self, major, minor, fix):
        self.major = major
        self.minor = minor
        self.fix = fix

    def __repr__(self):
        return 'Version(major={}, minor={}, fix={})'.format(self.major, self.minor, self.fix)

    def __str__(self):
        return '{}.{}.{}'.format(self.major, self.minor, self.fix)

    def __eq__(self, other):
        if not isinstance(other, Version):
            raise ValueError('Cannot compare version with object of type: {}'.format(type(other)))
        return self.major == other.major and self.minor == other.minor and self.fix == other.fix


class ScriptEnv():
    def __init__(self, scripts_root_path, log_level_name=_LOG_LEVEL):
        self._scripts_path = os.path.abspath(scripts_root_path)
        self._script_commands_path = os.path.abspath(os.path.join(self._scripts_path, _COMMANDS_DIR_NAME))
        self._engine_path = os.path.abspath(os.path.join(self._scripts_path, os.pardir))
        self._repo_path = os.path.abspath(os.path.join(self._engine_path, os.pardir))
        self._script_resources_path = os.path.abspath(os.path.join(self._scripts_path, _RESOURCES_DIR_NAME))
        self._script_tests_path = os.path.abspath(os.path.join(self._scripts_path, _TESTS_DIR_NAME))
        self._thirdparty_path = os.path.abspath(os.path.join(self._engine_path, _THIRDPARTY_DIR_NAME))
        self._log_level_name = log_level_name.upper()
        self._log_level = getattr(logging, self._log_level_name, None)
        if not isinstance(self._log_level, int):
            raise ValueError('Invalid log level: {}'.format(self._log_level_name))

    @property
    def scripts_path(self):
        return self._scripts_path

    @property
    def script_commands_path(self):
        return self._script_commands_path
    
    @property
    def engine_path(self):
        return self._engine_path

    @property
    def repo_path(self):
        return self._repo_path

    @property
    def script_resources_path(self):
        return self._script_resources_path

    @property
    def script_tests_path(self):
        return self._script_tests_path

    def get_logger(self, name, force_flush=True):
        logger = logging.getLogger(name)
        logger.setLevel(self._log_level)

        # Force flush
        if force_flush:
            h = logging.StreamHandler(sys.stdout)
            #h.flush = sys.stdout.flush
            #logger.addHandler(h)

        return logger

    def __str__(self):
        return '\t[ENGINE_PATH]\t{}\n\
                \r\t[REPO_PATH]\t{}\n\
                \r\t[SCRIPTS_PATH]\t{}\n\
                \r\t[SCRIPT_COMMANDS_PATH]\t{}\n\
                \r\t[SCRIPT_RESOURCES_PATH]\t{}\n\
                \r\t[SCRIPT_TESTS_PATH]\t{}'.format(
                self.engine_path,
                self.repo_path,
                self.scripts_path,
                self.script_commands_path,
                self.script_resources_path,
                self.script_tests_path)


### ENV SETUP ###
logging.basicConfig(format=_LOGGER_FORMAT)
SCRIPT_ENV = ScriptEnv(_SCRIPTS_ROOT)
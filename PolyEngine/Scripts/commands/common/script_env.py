import os
import logging
import sys

_COMMANDS_DIR_NAME = 'commands'
_RESOURCES_DIR_NAME = 'resources'
_TESTS_DIR_NAME = 'tests'
_LOG_LEVEL = 'DEBUG'

LOGGER_FORMAT = '[%(asctime)s] [%(levelname)s] [%(name)s]\t %(message)s'

class ScriptEnv():
    def __init__(self, scripts_root_path, log_level_name=_LOG_LEVEL):
        self._scripts_path = scripts_root_path
        self._script_commands_path = os.path.join(self._scripts_path, _COMMANDS_DIR_NAME)
        self._engine_path = os.path.abspath(os.path.join(self._scripts_path, os.pardir))
        self._repo_path = os.path.abspath(os.path.join(self._engine_path, os.pardir))
        self._script_resources_path = os.path.join(self._scripts_path, _RESOURCES_DIR_NAME)
        self._script_tests_path = os.path.join(self._scripts_path, _TESTS_DIR_NAME)
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
            h.flush = sys.stdout.flush
            logger.addHandler(h)

        return logger

    def __str__(self):
        return '[ENGINE_PATH]\t{}\n\
                \r[SCRIPTS_PATH]\t{}\n\
                \r[SCRIPT_COMMANDS_PATH]\t{}\n\
                \r[SCRIPT_RESOURCES_PATH]\t{}\n\
                \r[SCRIPT_TESTS_PATH]\t{}'.format(
                self.engine_path,
                self.scripts_path,
                self.script_commands_path,
                self.script_resources_path,
                self.script_tests_path)

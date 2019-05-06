import os

_COMMANDS_DIR_NAME = 'commands'
_RESOURCES_DIR_NAME = 'resources'

class ScriptEnv():
    def __init__(self, scripts_root_path):
        self._scripts_path = scripts_root_path
        self._commands_path = os.path.join(self._scripts_path, _COMMANDS_DIR_NAME)
        self._engine_path = os.path.abspath(os.path.join(self._scripts_path, os.pardir))
        self._resources_path = os.path.join(self._scripts_path, _RESOURCES_DIR_NAME)

    @property
    def scripts_path(self):
        return self._scripts_path

    @property
    def commands_path(self):
        return self._commands_path
    
    @property
    def engine_path(self):
        return self._engine_path

    @property
    def resources_path(self):
        return self._resources_path

    def __str__(self):
        return '[ENGINE_PATH]\t{}\n\
                \r[SCRIPTS_PATH]\t{}\n\
                \r[COMMANDS_PATH]\t{}\n\
                \r[RESOURCES_PATH]\t{}'.format(
                self.engine_path,
                self.scripts_path,
                self.commands_path,
                self.resources_path)

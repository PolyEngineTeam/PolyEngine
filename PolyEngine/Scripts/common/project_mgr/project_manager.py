import appdirs

import common
from .game_project import GameProject
from .engine_project import EngineProject

class ProjectManager():
    def __init__(self):
        self._logger = common.SCRIPT_ENV.get_logger(name='ProjectManager')

        dirs = appdirs.AppDirs("PolyEngine", "PolyEngine Team")
        self._user_data_path = dirs.user_data_dir
        self._user_cache_path = dirs.user_cache_dir
        self._projects = {}


        self._load_user_data()
        

    @property
    def projects(self):
        return self._projects

    def load_project(self, path):
        pass

    def create_project(self, path, project_name):
        pass

    def update_project(self, project_name):
        pass

    def _load_user_data(self):
        pass

    def _save_user_data(self):
        pass
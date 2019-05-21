import os

import common
from .project_base import ProjectBase, BUILD_DIR_NAME

class EngineProject(ProjectBase):
    def __init__(self, build_dir_name=BUILD_DIR_NAME):
        ProjectBase.__init__(self, common.SCRIPT_ENV.engine_path, 'PolyEngine', common.Version(0, 0, 1), build_dir_name)
        self._logger = common.SCRIPT_ENV.get_logger(name='EngineProject')

        if not os.path.isdir(self._root_path):
            raise NotADirectoryError('EngineProject root path [{}] does not exists'.format(self._root_path))
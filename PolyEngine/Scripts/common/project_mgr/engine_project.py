import os

import common
from .project_base import ProjectBase

class EngineProject(ProjectBase):
    def __init__(self, build_postfix=None):
        ProjectBase.__init__(self, common.SCRIPT_ENV.engine_path, 'PolyEngine', build_postfix)
        self._logger = common.SCRIPT_ENV.get_logger(name='EngineProject')

        if not os.path.isdir(self._root_path):
            raise NotADirectoryError('EngineProject root path [{}] does not exists'.format(self._root_path))
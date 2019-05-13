import os
import git

import common
from collections import namedtuple


class RepoManager():
    def __init__(self):
        self._patches_folder = os.path.join(common.SCRIPT_ENV.repo_path, 'PolyEngine', 'ThirdParty', 'patches')
        self._logger = common.SCRIPT_ENV.get_logger(name='RepoManager')    
        self._repo = git.Repo(common.SCRIPT_ENV.repo_path)

        SubmoduleInfo = namedtuple('SubmoduleInfo', ['submodule', 'patch_file_path'])

        self._submodules = {} 
        for submodule in self._repo.submodules:
            sub_name = os.path.basename(os.path.normpath(submodule.path))
            patch_file_path = os.path.join(self._patches_folder, '{}.patch'.format(sub_name))
            self._submodules[sub_name] = SubmoduleInfo(submodule, patch_file_path if os.path.isfile(patch_file_path) else None)
        

    def init_submodules(self):
        for sub_name, submodule_info in self._submodules.items():
            self._logger.info('Initializing submodule [{}] in path: {}'.format(sub_name, submodule_info.submodule.path))
            submodule_info.submodule.update(init=True, force=True)

            # Apply patch if needed
            if submodule_info.patch_file_path:
                self._logger.info('Applying patch to submodule {} found in {}'.format(sub_name, submodule_info.patch_file_path))
                sub_repo = submodule_info.submodule.module()
                sub_repo.git.reset(['--hard']) # Reset first
                sub_repo.git.apply([submodule_info.patch_file_path]) # Apply patch

    # def status(self):
    #     pass

    # def checkout(self, sha=None, tag=None, branch_name=None):
    #     pass

    # def push(self, remote, tags=False):
    #     pass

    # def add_remote(self, remote, url):
    #     pass

    # def add_tag(self, tag, sha=None):
    #     pass

    #def stage(self, file_pattern):
    #    pass

import argparse
import os
import git
from enum import Enum

script_location = os.path.dirname(os.path.abspath(__file__))
print('script_location:', script_location)
repo_path = os.path.abspath(os.sep.join([script_location, '..', '..']))
print('Using git repository location {}'.format(repo_path))
repo = git.Repo(repo_path)
repo_submodules = repo.submodules

patches_folder = os.sep.join([repo_path, 'PolyEngine', 'ThirdParty', 'patches'])

for submodule in repo_submodules:
    sub_name = os.path.basename(os.path.normpath(submodule.path))
    print('Initializing submodule [{}] in path: {}'.format(sub_name, submodule.path))
    submodule.update(init=True, force=True)
    
    patch_name = os.sep.join([patches_folder, '{}.patch'.format(sub_name)])

    # Apply patch if needed
    if os.path.isfile(patch_name):
        print('Applying patch to submodule {} found in {}'.format(sub_name,patch_name))
        sub_repo = submodule.module()
        sub_repo.git.reset(['--hard']) # Reset first
        sub_repo.git.apply([patch_name]) # Apply patch

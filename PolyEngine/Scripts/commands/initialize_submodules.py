import sys

import common.repo_manager as repo_manager

def execute(*args):
    repo_mgr = repo_manager.RepoManager()
    repo_mgr.init_submodules()

if __name__ == '__main__':
    execute(*sys.argv[1:])

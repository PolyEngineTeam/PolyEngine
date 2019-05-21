import argparse
import sys

import common.project_mgr as project_mgr

def execute(*args):
    PARSER = argparse.ArgumentParser(description='PolyEngine project updater')

    PARSER.add_argument("-b", "--build-dir-name", action='store', dest='build_dir_name',
                        default=project_mgr.project_base.BUILD_DIR_NAME,
                        help='name of the build folder')
    PARSER.add_argument('project_path', action='store', help='path to the project')

    ARGS = PARSER.parse_args([*args])

    mgr = project_mgr.GameProject(ARGS.project_path, create_if_absent=False, build_dir_name=ARGS.build_dir_name)
    mgr.update()

if __name__ == '__main__':
    execute(*sys.argv[1:])

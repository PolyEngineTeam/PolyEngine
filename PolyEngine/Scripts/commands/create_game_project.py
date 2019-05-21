import argparse
import sys

import common.project_mgr as project_mgr

def execute(*args):
    PARSER = argparse.ArgumentParser(description='PolyEngine project updater')

    PARSER.add_argument("-b", "--build-dir-name", action='store', dest='build_dir_name',
                        default=project_mgr.project_base.BUILD_DIR_NAME,
                        help='name of the build folder')
    PARSER.add_argument('project_path', action='store', help='Path to the project root')
    PARSER.add_argument('project_name', action='store', help='Name of the project')

    ARGS = PARSER.parse_args([*args])


    mgr = project_mgr.GameProject(ARGS.project_path, ARGS.project_name, create_if_absent=True, build_dir_name=ARGS.build_dir_name)

if __name__ == '__main__':
    execute(*sys.argv[1:])
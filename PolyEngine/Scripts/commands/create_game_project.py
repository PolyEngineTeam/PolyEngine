import argparse
import sys

import common.project_mgr as project_mgr

def execute(*args):
    PARSER = argparse.ArgumentParser(description='PolyEngine project updater')

    PARSER.add_argument("-b", "--build-postfix", action='store', dest='build_postfix',
                        default=None,
                        help='postfix of the build folder')
    PARSER.add_argument('project_path', action='store', help='Path to the project root')
    PARSER.add_argument('project_name', action='store', help='Name of the project')

    ARGS = PARSER.parse_args([*args])


    mgr = project_mgr.GameProject(ARGS.project_path, ARGS.project_name, create_if_absent=True, build_postfix=ARGS.build_postfix)
    mgr.update()

if __name__ == '__main__':
    execute(*sys.argv[1:])
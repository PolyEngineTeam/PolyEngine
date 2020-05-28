import argparse
import sys

import common.project_mgr as project_mgr

def execute(*args):
    PARSER = argparse.ArgumentParser(description='PolyEngine game build folder cleaner')

    PARSER.add_argument("-b", "--build-postfix", action='store', dest='build_postfix',
                        default=None,
                        help='postfix of the game build folder')
    PARSER.add_argument('project_path', action='store', help='Path to the project root')

    ARGS = PARSER.parse_args([*args])

    mgr = project_mgr.GameProject(ARGS.project_path, create_if_absent=False, build_postfix=ARGS.build_postfix)
    mgr.clean_build()

if __name__ == '__main__':
    execute(*sys.argv[1:])

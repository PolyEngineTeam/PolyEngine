import argparse
import sys

import common.project_mgr as project_mgr

def execute(*args):
    PARSER = argparse.ArgumentParser(description='PolyEngine project build folder cleaner')

    PARSER.add_argument("-b", "--build-postfix", action='store', dest='build_postfix',
                        default=None,
                        help='postfix of the build folder')

    ARGS = PARSER.parse_args([*args])

    mgr = project_mgr.EngineProject(build_postfix=ARGS.build_postfix)
    mgr.clean_build()

if __name__ == '__main__':
    execute(*sys.argv[1:])

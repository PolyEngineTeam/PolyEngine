import argparse
import os
import subprocess
import time
import sys
import docker

import common
import common.docker_manager as docker_mgr

DOCKERFILES_FOLDER = 'dockerfiles'
DOCKERFILES_PATH = os.path.join(common.SCRIPT_ENV.script_resources_path, DOCKERFILES_FOLDER)

def execute(script_env, *args):
    mgr = docker_mgr.DockerManager(dockerfile_dir=DOCKERFILES_PATH)
    supported_os = []
    supported_variant = []
    for key, values in mgr.supported_configurations.items():
        supported_os.append(key)
        for value in values:
            supported_variant.append(value)

    parser = argparse.ArgumentParser(description='PolyEngine project management tool')
    parser.add_argument("--os", action='store',
                        default=supported_os[0],
                        choices=supported_os,
                        help='Operating system')
    parser.add_argument("--variant", action='store',
                        default=supported_variant[0],
                        choices=supported_variant,
                        help='Variant')
    args = parser.parse_args([*args])

    if args.variant not in mgr.supported_configurations[args.os]:
        raise ValueError('Unsupported configuration {}-{}! Supported configurations: {}'.format(
            args.os, args.variant, mgr.supported_configurations
        ))

    mgr.get_image(args.os, args.variant).start_shell()

if __name__ == '__main__':
    execute(common.SCRIPT_ENV, *sys.argv[1:])

    

    
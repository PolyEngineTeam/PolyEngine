import argparse
import os
import subprocess
import time

import docker

SUPPORTED_OS = ['ubuntu18']
SUPPORTED_COMPILER = ['gcc8', 'clang5']
SCRIPT_LOCATION = os.path.dirname(__file__)
REPO_PATH = os.path.abspath(os.sep.join([SCRIPT_LOCATION, '..', '..']))
DOCKER_FILES_ROOT = os.path.abspath(os.sep.join([SCRIPT_LOCATION, 'Docker']))

def get_image(image_name):
    client = docker.from_env()
    available_images = client.images.list()
    for img in available_images:
        if image_name + ':latest' in img.tags:
            return img
    return None

def run_image(system, compiler, repo_root):
    base_image_name = '{}-base'.format(system)
    image_name = '{}-{}'.format(system, compiler)
    base_dockerfile_path = os.sep.join([DOCKER_FILES_ROOT, base_image_name + '.dockerfile'])
    dockerfile_path = os.sep.join([DOCKER_FILES_ROOT, image_name + '.dockerfile'])

    client = docker.from_env()

    if not os.path.isfile(base_dockerfile_path):
        raise FileNotFoundError('Dockerfile {}.dockerfile not found.'.format(base_image_name))
    print('Building base image: {}:latest. This may take a while...'.format(base_image_name))
    client.images.build(path=DOCKER_FILES_ROOT, dockerfile=base_dockerfile_path, tag=base_image_name, rm=True)[0]
    
    if not os.path.isfile(dockerfile_path):
        raise FileNotFoundError('Dockerfile {}.dockerfile not found.'.format(image_name))
    print('Building image: {}:latest.'.format(image_name))
    img = client.images.build(path=DOCKER_FILES_ROOT, dockerfile=dockerfile_path, tag=image_name, rm=True)[0]

    time.sleep(2) # Docker somtimes fails when run is called right after build. Just wait for 2s.

    print('Starting docker image {}:latest ...'.format(image_name))
    #client.containers.run(image_name, command='/bin/bash', tty=True, stdin_open=True, auto_remove=True, remove=True, detach=False)
    subprocess.run("docker container run -it --rm -w /root/workspace -v {}:/root/workspace {}".format(REPO_PATH, image_name + ':latest'), shell=True, check=True)

if __name__ == "__main__":

    parser = argparse.ArgumentParser(description='PolyEngine project management tool')
    parser.add_argument("--os", action='store',
                        default=SUPPORTED_OS[0],
                        choices=SUPPORTED_OS,
                        help='Operating system')
    parser.add_argument("--compiler", action='store',
                        default=SUPPORTED_COMPILER[0],
                        choices=SUPPORTED_COMPILER,
                        help='Compiler')
    args = parser.parse_args()

    run_image(args.os, args.compiler, REPO_PATH)

    
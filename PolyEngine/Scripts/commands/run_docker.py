import argparse
import os
import subprocess
import time
import sys
import docker

try:
    import commands.common as common
except:
    try:
        import common
    except:
        raise ImportError("Cannot import common lib!")

SUPPORTED_OS = ['ubuntu18']
SUPPORTED_COMPILER = ['gcc8', 'clang5']

def get_image(image_name):
    client = docker.from_env()
    available_images = client.images.list()
    for img in available_images:
        if image_name + ':latest' in img.tags:
            return img
    return None

def run_image(script_env, system, compiler):
    docker_files_path = os.path.join(script_env.script_resources_path, 'dockerfiles')

    base_image_name = '{}-base'.format(system)
    image_name = '{}-{}'.format(system, compiler)
    base_dockerfile_path = os.path.join(docker_files_path, base_image_name + '.dockerfile')
    dockerfile_path = os.path.join(docker_files_path, image_name + '.dockerfile')

    client = docker.from_env()

    if not os.path.isfile(base_dockerfile_path):
        raise FileNotFoundError('Dockerfile {}.dockerfile not found.'.format(base_image_name))
    print('Building base image: {}:latest. This may take a while...'.format(base_image_name))
    client.images.build(path=docker_files_path, dockerfile=base_dockerfile_path, tag=base_image_name, rm=True)[0]
    
    if not os.path.isfile(dockerfile_path):
        raise FileNotFoundError('Dockerfile {}.dockerfile not found.'.format(image_name))
    print('Building image: {}:latest.'.format(image_name))
    img = client.images.build(path=docker_files_path, dockerfile=dockerfile_path, tag=image_name, rm=True)[0]

    time.sleep(2) # Docker somtimes fails when run is called right after build. Just wait for 2s.

    print('Starting docker image {}:latest ...'.format(image_name))
    #client.containers.run(image_name, command='/bin/bash', tty=True, stdin_open=True, auto_remove=True, remove=True, detach=False)
    subprocess.run("docker container run -it --rm -w /root/workspace -v {}:/root/workspace {}".format(script_env.repo_path, image_name + ':latest'), shell=True, check=True)


def execute(script_env, *args):
    print('Using execute!')
    parser = argparse.ArgumentParser(description='PolyEngine project management tool')
    parser.add_argument("--os", action='store',
                        default=SUPPORTED_OS[0],
                        choices=SUPPORTED_OS,
                        help='Operating system')
    parser.add_argument("--compiler", action='store',
                        default=SUPPORTED_COMPILER[0],
                        choices=SUPPORTED_COMPILER,
                        help='Compiler')
    args = parser.parse_args([*args])

    run_image(script_env, args.os, args.compiler)

if __name__ == '__main__':
    execute(common.SCRIPT_ENV, *sys.argv[1:])

    

    
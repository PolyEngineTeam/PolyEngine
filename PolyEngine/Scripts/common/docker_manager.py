import argparse
import os
import subprocess
import time
import sys
import docker
import glob
import re
from collections import defaultdict 

from .env import SCRIPT_ENV

DOCKERFILE_EXT = '.dockerfile'
DOCKERFILE_NAME_REGEX = '(\\S*)-(\\S*)\\' + DOCKERFILE_EXT

DOCKERFILE_FROM = 'FROM'
DOCKERFILE_FROM_REGEX = DOCKERFILE_FROM + ' (\\S*)'
DOCKERFILE_DEPENDENCY_REGEX = '(\\S*):(\\S*)'
DOCKER_STREAM_REGEX = '{\\"stream\\":\\"(.*)\\\\n\\"}'

DOCKER_IMAGE_LATEST_VERSION = 'latest'

ABSTRACT_VARIANTS = [ 'base' ]

class DockerContainer():
    def __init__(self, image, image_name):
        self._image = image
        self._image_name = image_name

    def start_shell(self):
        self.run_it_cmd('/bin/bash')

    def run_it_cmd(self, cmd):
        subprocess.run("docker container run -it --rm -w /root/workspace -v {}:/root/workspace {} {}"\
        .format(SCRIPT_ENV.repo_path, self._image_name, cmd), shell=True, check=False)

    def run_cmd(self, cmd):
        subprocess.run("docker container run --rm -w /root/workspace -v {}:/root/workspace {} {}"\
        .format(SCRIPT_ENV.repo_path, self._image_name, cmd), shell=False, check=True)

class DockerFile():
    def __init__(self, path):
        if not os.path.isfile(path):
            raise ValueError('Cannot find docker file under path: {}'.format(path))
        
        self._path = os.path.abspath(path)
        self._file_name = os.path.basename(self._path)
        self._image_name = os.path.splitext(self._file_name)[0]

        match = re.fullmatch(DOCKERFILE_NAME_REGEX, self._file_name)
        self._system = match.group(1)
        self._variant = match.group(2)

        self._dependent_image = self._get_dockerfile_dependency()

    def __repr__(self):
        return 'DockerFile({})'.format(self._image_name)

    @property
    def path(self):
        return self._path

    @property
    def system(self):
        return self._system

    @property
    def variant(self):
        return self._variant

    @property
    def dependency(self):
        return self._dependent_image

    @property
    def image_name(self):
        return self._image_name

    def _get_dockerfile_dependency(self):
        with open(self.path, 'r') as df:
            for line in df:
                match = re.match(DOCKERFILE_FROM_REGEX, line)
                if match:
                    dependency = match.group(1)

                    # Try to match version if provided
                    match2 = re.match(DOCKERFILE_DEPENDENCY_REGEX, dependency)
                    if match2:
                        name = match2.group(1)
                        version = match2.group(2)
                        return (name, version)
                    
                    # return latest version if none was provided
                    return (dependency, DOCKER_IMAGE_LATEST_VERSION)
                
            return None
    

class DockerManager():
    def __init__(self, dockerfiles=None, dockerfile_dir=None):
        self._docker_client = docker.from_env()
        self._docker_api_client = docker.APIClient()
        self._logger = SCRIPT_ENV.get_logger('DockerManager')
        
        dockerfiles = []
        if dockerfiles:
            dockerfiles += [ DockerFile(df) for df in dockerfiles if os.path.isfile(df) ]
        if dockerfile_dir and os.path.isdir(dockerfile_dir):
            dockerfiles += [ DockerFile(df) for df in glob.glob(os.path.join(dockerfile_dir, '*' + DOCKERFILE_EXT), recursive=True) ]
        if not dockerfiles:
            raise ValueError('You have to provide correct dockerfiles and/or valid dockerfile_dir.')
        
        self._dockerfiles = {}
        self._supported_configurations = defaultdict(set)
        for df in dockerfiles:
            self._dockerfiles[df.image_name] = df
            
            if df.variant not in ABSTRACT_VARIANTS:
                self._supported_configurations[df.system].add(df.variant)
            
            print(df.dependency)
        
        print(self._supported_configurations)
        print(self._dockerfiles)
        
        self._image_registry = {}

    @property
    def supported_configurations(self):
        return self._supported_configurations

    def get_image(self, system, variant):
        if variant in ABSTRACT_VARIANTS:
            raise ValueError('Trying to instantiate abstract image!')

        image_name = self._encode_image_name(system, variant)
        self._logger.debug('Creating image: [{}]'.format(image_name))

        image = self._get_existing_image(image_name)
        if not image:
            image = self._build_new_image(image_name)
        return DockerContainer(image, image_name)

    def _build_new_image(self, image_name):
        self._logger.debug('Building image [{}]'.format(image_name))
        df = self._dockerfiles.get(image_name)
        dir_path, filename = os.path.split(df.path)

        if df.dependency and \
            not self._get_existing_image(df.dependency[0], DOCKER_IMAGE_LATEST_VERSION) \
            and df.dependency[0] in self._dockerfiles:
            self._logger.debug('Missing dependency: [{}:{}] Trying to build it...'.format(df.dependency[0], df.dependency[1]))
            self._build_new_image(df.dependency[0])

        self._logger.debug('Build [{}] started...'.format(image_name))

        out_gen = self._docker_api_client.build(path=dir_path, dockerfile=filename, tag=df.image_name, rm=True)
        for line in (l.decode('UTF-8') for l in out_gen):
            match = re.match(DOCKER_STREAM_REGEX, line)
            if match:
                matched_str = match.group(1).replace('\\r', '\r').replace('\\n', '\n').replace('---\\u003e', '>')
                print(matched_str)
        #image = self._docker_client.images.build(path=dir_path, dockerfile=filename, tag=df.image_name, rm=True)[0]
        image = self._get_existing_image(image_name)
        if not image:
            raise RuntimeError('Failed to build image [{}]'.format(image_name))
        self._logger.debug('Done')
        return image

    def _encode_image_name(self, system, variant):
        return system + '-' + variant

    def _get_existing_image(self, image_name, version=DOCKER_IMAGE_LATEST_VERSION):
        available_images = self._docker_client.images.list()
        for img in available_images:
            full_img_name = image_name + ':' + version
            if full_img_name in img.tags:
                return img
        return None
import argparse
import os
import shutil
import fileinput
import json
from enum import Enum

class ActionType(Enum):
    CREATE = 'Create',
    UPDATE = 'Update'

    def __str__(self):
        return self.name

# Custom action for project creation
class CreateProjectAction(argparse.Action):
    def __init__(self, option_strings, dest, nargs, **kwargs):
        if nargs is not 2:
            raise ValueError("Nargs must be 2 !")
        super(CreateProjectAction, self).__init__(option_strings, dest, nargs, **kwargs)
    def __call__(self, parser, namespace, values, option_string=None):
        setattr(namespace, self.dest, ActionType.CREATE)
        setattr(namespace, self.metavar[0], values[0])
        setattr(namespace, self.metavar[1], values[1])

class UpdateProjectAction(argparse.Action):
    def __init__(self, option_strings, dest, nargs, **kwargs):
        if nargs is not 1:
            raise ValueError("Nargs must be 1 !")
        super(UpdateProjectAction, self).__init__(option_strings, dest, nargs, **kwargs)
    def __call__(self, parser, namespace, values, option_string=None):
        setattr(namespace, self.dest, ActionType.UPDATE)
        setattr(namespace, self.metavar, values[0])

# Functions
# Create project function

# Setup:
# $PROJECT_NAME$ - name of the project
# $PROJECT_PATH$ - relative path of the project (game) directory (relative to root project dir)
# $GAME_CLASS_NAME$ - name of the game class
# $ENGINE_DIR$ - absolute path to engine root directory

def get_cmake_path(path):
    return path.replace('\\', '/')

def replace_tags_in_file(file_to_search, tags_and_values):
    print('Replacing tags in', file_to_search)
    for (tag, val) in tags_and_values:
        print('\tchanging', tag, 'to', val)

    with fileinput.FileInput(file_to_search, inplace=True) as file:
        for line in file:
            for (tag, val) in tags_and_values:
                line = line.replace(tag, val)
            print(line, end='')

def run_cmake(path, build_dir_name):
    build_dir_path = os.sep.join([path, build_dir_name])
    if not os.path.exists(build_dir_path):
        os.makedirs(build_dir_path)
    
    # Run cmake update (using undocumented parameters that work for some reason, src: http://cprieto.com/posts/2016/10/cmake-out-of-source-build.html)
    if os.name == 'nt':
        os.system('cmake -G "Visual Studio 15 2017 Win64" -H{} -B{}'.format(get_cmake_path(path), get_cmake_path(build_dir_path)))
    else:
        os.system('cmake -H{} -B{}'.format(get_cmake_path(path), get_cmake_path(build_dir_path)))

def create_project_file(path, proj_name):
    data = {}
    data['ProjectName'] = proj_name
    with open(os.sep.join([path, proj_name + '.proj.json']), 'w') as outfile:
        json.dump(data, outfile)

def read_project_file(path):
    for file in os.listdir(path):
        if file.endswith(".proj.json"):
            with open(os.sep.join([path, file])) as json_file:  
                    data = json.load(json_file)
            return data

def update_config_files(path, name, engine_path, is_new_config):
    project_path = os.sep.join([path, name])
    project_sources_path = os.sep.join([project_path, 'Src'])

    scripts_data_path = os.sep.join([engine_path, 'Scripts'])
    game_hpp_output_path = os.sep.join([project_sources_path, 'Game.hpp'])
    game_cpp_output_path = os.sep.join([project_sources_path, 'Game.cpp'])
    cmake_sln_output_path = os.sep.join([path, 'CMakeLists.txt'])
    cmake_proj_output_path = os.sep.join([project_path, 'CMakeLists.txt'])

    if is_new_config:
        shutil.copy(os.sep.join([scripts_data_path, 'Game.hpp.in']), game_hpp_output_path)
        shutil.copy(os.sep.join([scripts_data_path, 'Game.cpp.in']), game_cpp_output_path)
    shutil.copy(os.sep.join([scripts_data_path, 'Sln-CMakeLists.txt.in']), cmake_sln_output_path)
    shutil.copy(os.sep.join([scripts_data_path, 'Proj-CMakeLists.txt.in']), cmake_proj_output_path)

    if is_new_config:
        replace_tags_in_file(game_hpp_output_path, [('$GAME_CLASS_NAME$', 'Game')])
        replace_tags_in_file(game_cpp_output_path, [('$GAME_CLASS_NAME$', 'Game')])
    # Cmake needs paths with '/' (instead of '\') regardles of platform
    replace_tags_in_file(cmake_sln_output_path, [('$PROJECT_PATH$', name), ('$ENGINE_DIR$', get_cmake_path(os.path.abspath(engine_path)))]) 
    replace_tags_in_file(cmake_proj_output_path, [('$PROJECT_NAME$', name)])

def create_project(name, path, engine_path):
    print('Creating project', name, 'in', path, 'with engine at', engine_path)

    if os.path.exists(path):
        raise Exception('Path', path, 'already exists. Cannot create project there!')
    
    path = os.path.abspath(path)

    # Create all necessary directories
    os.makedirs(path)
    project_path = os.sep.join([path, name])
    os.makedirs(project_path)
    project_resources_path = os.sep.join([project_path, 'Res'])
    os.makedirs(project_resources_path)
    project_sources_path = os.sep.join([project_path, 'Src'])
    os.makedirs(project_sources_path)

    update_config_files(path, name, engine_path, True)

    create_project_file(path, name)
    run_cmake(path, 'Build')

def update_project(path, engine_path):
    print('Updating project at', path, 'with engine at', engine_path)

    if not os.path.exists(path):
        raise Exception('Path', path, 'does not exists. Cannot update project there!')

    name = read_project_file(path)['ProjectName']
    print('Project name:', name)

    update_config_files(path, name, engine_path, False)

    run_cmake(path, 'Build')

#################### SCRIPT START ####################
if __name__ == "__main__":
    PARSER = argparse.ArgumentParser(description='PolyEngine project management tool')

    PARSER.add_argument("-e", "--engine", action='store', dest='enginePath',
                        default='..',
                        metavar='engine_path',
                        help='provide custom engine path')

    MTX = PARSER.add_mutually_exclusive_group()
    MTX.add_argument('-c', '--create', action=CreateProjectAction, dest='action_to_perform',
                        nargs=2, metavar=('project_path', 'project_name'),
                        help='create project of given name at provided path')
    MTX.add_argument('-u', '--update', action=UpdateProjectAction, dest='action_to_perform',
                        nargs=1, metavar='project_path',
                        help='update project at given path')


    ARGS = PARSER.parse_args()

    if ARGS.action_to_perform == ActionType.CREATE:
        create_project(ARGS.project_name, ARGS.project_path, ARGS.enginePath)
    elif ARGS.action_to_perform == ActionType.UPDATE:
        update_project(ARGS.project_path, ARGS.enginePath)

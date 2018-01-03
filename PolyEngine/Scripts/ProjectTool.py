import argparse
import os
import shutil
import sys
import fileinput
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

def ReplaceTagsInFile(fileToSearch, tagsAndValues):
    print('Replacing tags in', fileToSearch)
    for (tag, val) in tagsAndValues:
        print('\tchanging', tag, 'to', val)

    with fileinput.FileInput(fileToSearch, inplace=True) as file:
        for line in file:
            for (tag, val) in tagsAndValues:
                line = line.replace(tag, val)
            print(line, end='')

def RunCmake(path, buildDir):
    # Run cmake update (using undocumented parameters that work for some reason, src: http://cprieto.com/posts/2016/10/cmake-out-of-source-build.html)
    if os.name == 'nt':
        os.system('cmake -G "Visual Studio 14 2015 Win64" -H {} -B {}'.format(path, buildDir))
    else:
        os.system('cmake -H {} -B {}'.format(path, buildDir))

def CreateProject(name, path, enginePath):
    print('Creating project', name, 'in', path, 'with engine at', enginePath)

    if os.path.exists(path):
        raise Exception('Path', path, 'already exists. Cannot create project there!')
    
    path = os.path.abspath(path)

    os.makedirs(path)

    projectPath = os.sep.join([path, name])
    os.makedirs(projectPath)
    projectResourcesPath = os.sep.join([projectPath, 'Res'])
    os.makedirs(projectResourcesPath)
    projectSourcesPath = os.sep.join([projectPath, 'Src'])
    os.makedirs(projectSourcesPath)

    scriptsDataPath = os.sep.join([enginePath, 'Scripts'])
    gameHppOutputPath = os.sep.join([projectSourcesPath, 'Game.hpp'])
    gameCppOutputPath = os.sep.join([projectSourcesPath, 'Game.cpp'])
    cmakeSlnOutputPath = os.sep.join([path, 'CMakeLists.txt'])
    cmakeProjOutputPath = os.sep.join([projectPath, 'CMakeLists.txt'])

    shutil.copy(os.sep.join([scriptsDataPath, 'Game.hpp.in']), gameHppOutputPath)
    shutil.copy(os.sep.join([scriptsDataPath, 'Game.cpp.in']), gameCppOutputPath)
    shutil.copy(os.sep.join([scriptsDataPath, 'Sln-CMakeLists.txt.in']), cmakeSlnOutputPath)
    shutil.copy(os.sep.join([scriptsDataPath, 'Proj-CMakeLists.txt.in']), cmakeProjOutputPath)

    ReplaceTagsInFile(gameHppOutputPath, [('$GAME_CLASS_NAME$', 'Game')])
    ReplaceTagsInFile(gameCppOutputPath, [('$GAME_CLASS_NAME$', 'Game')])
    # Cmake needs paths with '/' (instead of '\') regardles of platform
    ReplaceTagsInFile(cmakeSlnOutputPath, [('$PROJECT_PATH$', name), ('$ENGINE_DIR$', os.path.abspath(enginePath).replace('\\', '/'))]) 
    ReplaceTagsInFile(cmakeProjOutputPath, [('$PROJECT_NAME$', name)])

    buildDirPath = os.sep.join([path, 'Build'])
    os.makedirs(buildDirPath)
    RunCmake(path, buildDirPath.replace('\\', '/'))

def UpdateProject(path, enginePath):
    print('Updating project at', path, 'with engine at', enginePath)

    if not os.path.exists(path):
        raise Exception('Path', path, 'does not exists. Cannot update project there!')

    RunCmake(path, os.sep.join([path, 'Build']).replace('\\', '/'))

#################### SCRIPT START ####################
if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='PolyEngine project management tool')

    parser.add_argument("-e", "--engine", action='store', dest='enginePath',
                        default='..',
                        metavar='engine_path',
                        help='provide custom engine path')

    mtx = parser.add_mutually_exclusive_group()
    mtx.add_argument('-c', '--create', action=CreateProjectAction, dest='actionToPerform',
                        nargs=2, metavar=('project_path', 'project_name'),
                        help='create project of given name at provided path')
    mtx.add_argument('-u', '--update', action=UpdateProjectAction, dest='actionToPerform',
                        nargs=1, metavar='project_path',
                        help='update project at given path')


    args = parser.parse_args()

    if args.actionToPerform == ActionType.CREATE:
        CreateProject(args.project_name, args.project_path, args.enginePath)
    elif args.actionToPerform == ActionType.UPDATE:
        UpdateProject(args.project_path, args.enginePath)

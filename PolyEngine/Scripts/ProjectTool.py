import argparse
import os
import shutil
import fileinput
import json
import subprocess
from enum import Enum
from operator import attrgetter
import xml.etree.ElementTree as ET

# FIXME should be 'master'
DEFAULT_BRANCH = 'dev'


class ActionType(Enum):
    CREATE = 'Create',
    UPDATE = 'Update',
    BUMP = 'Bump'

    def __str__(self):
        return self.name


class Tag:
    def __init__(self, version_string):
        try:
            splitted = version_string.split('.')
            self.major = int(splitted[0])
            self.minor = int(splitted[1])
            self.fix = int(splitted[2])
        except ValueError:
            raise RuntimeError('Invalid tag found in repository: {}'.format(version_string))

    def __str__(self):
        return '{}.{}.{}'.format(self.major, self.minor, self.fix)


# Custom action for project creation
class CreateProjectAction(argparse.Action):
    def __init__(self, option_strings, dest, nargs, **kwargs):
        if nargs != 2:
            raise ValueError("Nargs must be 2 !")
        super(CreateProjectAction, self).__init__(option_strings, dest, nargs, **kwargs)

    def __call__(self, parser, namespace, values, option_string=None):
        setattr(namespace, self.dest, ActionType.CREATE)
        setattr(namespace, self.metavar[0], values[0])
        setattr(namespace, self.metavar[1], values[1])


class UpdateProjectAction(argparse.Action):
    def __init__(self, option_strings, dest, nargs, **kwargs):
        if nargs != 1:
            raise ValueError("Nargs must be 1 !")
        super(UpdateProjectAction, self).__init__(option_strings, dest, nargs, **kwargs)

    def __call__(self, parser, namespace, values, option_string=None):
        setattr(namespace, self.dest, ActionType.UPDATE)
        setattr(namespace, self.metavar, values[0])


class BumpVersionAction(argparse.Action):
    def __init__(self, option_strings, dest, nargs, **kwargs):
        if nargs != 1:
            raise ValueError("Nargs must be 1 !")
        super(BumpVersionAction, self).__init__(option_strings, dest, nargs, **kwargs)

    def __call__(self, parser, namespace, values, option_string=None):
        setattr(namespace, self.dest, ActionType.BUMP)
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


def xml_indent(elem, level=0):
    i = "\n" + level*"  "
    if len(elem):
        if not elem.text or not elem.text.strip():
            elem.text = i + "  "
        if not elem.tail or not elem.tail.strip():
            elem.tail = i
        for elem in elem:
            xml_indent(elem, level+1)
        if not elem.tail or not elem.tail.strip():
            elem.tail = i
    else:
        if level and (not elem.tail or not elem.tail.strip()):
            elem.tail = i


def patch_usr_proj(path, proj_name):
    usr_proj_path = os.sep.join([path, 'Build', proj_name, proj_name + '.vcxproj.user'])
    xml_namespace = { 'ns' : 'http://schemas.microsoft.com/developer/msbuild/2003' }
    namespace_str = '{' + xml_namespace['ns'] + '}'
    ET.register_namespace('', xml_namespace['ns'])
    
    property_group_conditions_to_add = ["'$(Configuration)|$(Platform)'=='Debug|x64'",
                                        "'$(Configuration)|$(Platform)'=='DebugFast|x64'",
                                        "'$(Configuration)|$(Platform)'=='Release|x64'",
                                        "'$(Configuration)|$(Platform)'=='Testing|x64'"]

    if os.path.exists(usr_proj_path):
        # Load file if exists
        print('File', usr_proj_path, 'found, parsing xml data...')
        xml_data = ET.parse(usr_proj_path)
    else:
        # Create file if doesn't exist
        print('File', usr_proj_path, 'not found, creating it...')
        xml_data = ET.ElementTree(ET.fromstring('<Project xmlns="http://schemas.microsoft.com/developer/msbuild/2003"></Project>'))
    
    root = xml_data.getroot()
    print('Patching', usr_proj_path)

    # find all present property groups
    for project_property_group in root.findall('ns:PropertyGroup', xml_namespace):
        print('Found property group:', project_property_group.attrib['Condition'])
        property_group_conditions_to_add.remove(project_property_group.attrib['Condition'])

    # add all missing property groups
    for condition in property_group_conditions_to_add:
        print('Creating missing property group:', condition)
        el = ET.SubElement(root, namespace_str + 'PropertyGroup')
        el.attrib['Condition'] = condition

    # patch all property groups info
    for project_property_group in root.findall('ns:PropertyGroup', xml_namespace):
        print('Patching property group:', project_property_group.attrib['Condition'])
        
        dbg_cmd = project_property_group.find('ns:LocalDebuggerCommand', xml_namespace)
        if dbg_cmd is None:
            dbg_cmd = ET.SubElement(project_property_group, namespace_str + 'LocalDebuggerCommand')
        dbg_cmd.text = 'PolyStandalone.exe'
        print('\tset LocalDebuggerCommand to', dbg_cmd.text)
        
        dbg_work_dir = project_property_group.find('ns:LocalDebuggerWorkingDirectory', xml_namespace)
        if dbg_work_dir is None:
            dbg_work_dir = ET.SubElement(project_property_group, namespace_str + 'LocalDebuggerWorkingDirectory')
        dbg_work_dir.text = '$(OutputPath)'
        print('\tset LocalDebuggerWorkingDirectory to', dbg_work_dir.text)

        dbg_flavor = project_property_group.find('ns:DebuggerFlavor', xml_namespace)
        if dbg_flavor is None:
            dbg_flavor = ET.SubElement(project_property_group, namespace_str + 'DebuggerFlavor')
        dbg_flavor.text = 'WindowsLocalDebugger'
        print('\tset DebuggerFlavor to', dbg_flavor.text)

    xml_indent(root)
    xml_data.write(usr_proj_path, encoding='utf-8', xml_declaration=True)


def run_cmake(path, build_dir_name, proj_name):
    build_dir_path = os.sep.join([path, build_dir_name])
    if not os.path.exists(build_dir_path):
        os.makedirs(build_dir_path)
    
    # Run cmake update with 64bit arch (using undocumented parameters that work for some reason,
    # src: http://cprieto.com/posts/2016/10/cmake-out-of-source-build.html)
    if os.name == 'nt':
        os.system('cmake -A x64 -H{} -B{}'.format(get_cmake_path(path), get_cmake_path(build_dir_path)))
    else:
        os.system('cmake -H{} -B{}'.format(get_cmake_path(path), get_cmake_path(build_dir_path)))
    
    # Patch project proj.user file to contain proper runtime info
    if os.name == 'nt':
        patch_usr_proj(path, proj_name)


def create_project_file(path, proj_name, tag):
    data = {
        'ProjectName': proj_name,
        'EngineMajorVersion': tag.major
    }
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
    cmake_sln_output_path_tuples = [
        ('$PROJECT_PATH$', name),
        ('$PROJECT_NAME$', name),
        ('$ENGINE_DIR$', get_cmake_path(os.path.abspath(engine_path)))
    ]
    replace_tags_in_file(cmake_sln_output_path, cmake_sln_output_path_tuples)
    replace_tags_in_file(cmake_proj_output_path, [('$PROJECT_NAME$', name)])

    create_fast_update_script(engine_path, path)


def create_fast_update_script(engine_path, proj_path):
    abs_engine_path = os.path.abspath(engine_path)
    script_path = os.sep.join([abs_engine_path, 'Scripts', 'ProjectTool.py'])
    
    if os.name == 'nt':
        file_content = 'py {} -e {} -u {}'.format(script_path, abs_engine_path, proj_path)
        file_name = os.sep.join([proj_path, 'quick_update.bat'])
    else:
        file_content = '#!/bin/bash\npython3.6 {} -e {} -u {}'.format(script_path, abs_engine_path, proj_path)
        file_name = os.sep.join([proj_path, 'quick_update.sh'])

    with open(file_name, 'w') as outfile:
        outfile.write(file_content)

    if os.name == 'posix':
        os.system("chmod +x {}".format(file_name))
        

def create_project(name, path, engine_path):
    print('Creating project', name, 'in', path, 'with engine at', engine_path)

    tag = get_current_tag(engine_path)
    checkout(engine_path, tag) #or maybe checkout latest major version instead?

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

    create_project_file(path, name, tag)
    run_cmake(path, 'Build', name)


def update_project(path, engine_path, checkout_required):
    project_file = read_project_file(path)
    if checkout_required:
        major_version = project_file.get('EngineMajorVersion', None)
        if major_version is not None:
            tags = get_tags(engine_path)
            tag = get_latest_tag_with_major(tags, major_version)
            checkout(engine_path, tag)
        else:
            raise RuntimeError("There is no 'EngineMajorVersion' defined in game project file")
    print('Updating project at', path, 'with engine at', engine_path)

    if not os.path.exists(path):
        raise RuntimeError('Path', path, 'does not exists. Cannot update project there!')

    name = project_file['ProjectName']
    print('Project name:', name)

    update_config_files(path, name, engine_path, False)

    run_cmake(path, 'Build', name)


def get_latest_tag_with_major(tags, major):
    tags = [tag for tag in tags if tag.major == major]
    minor = max(tags, key=attrgetter('minor')).minor
    tags = [tag for tag in tags if tag.minor == minor]
    fix = max(tags, key=attrgetter('fix')).fix
    return Tag('{}.{}.{}'.format(major, minor, fix))


def get_tags(engine_path):
    checkout(engine_path, DEFAULT_BRANCH)
    tags_raw = subprocess.check_output('cd {} && git -c pager.tag=false tag'.format(engine_path), shell=True)\
        .decode('utf-8')
    print('Available engine versions: {}'.format(tags_raw))
    return [Tag(tag_raw.strip()) for tag_raw in tags_raw.split('\n') if tag_raw]


def bump_version(path, engine_path):
    project_file = read_project_file(path)
    name = project_file['ProjectName']
    tag = get_current_tag(engine_path)
    create_project_file(path, name, tag)


def get_current_tag(engine_path):
    tag_raw = subprocess.check_output('cd {} && git describe --tags --abbrev=0'.format(engine_path), shell=True)\
        .decode('utf-8').strip()
    print('Current engine version: {}'.format(tag_raw))
    return Tag(tag_raw)


def checkout(engine_path, identifier):
    print('Checking out engine from branch/tag: {}'.format(identifier))
    os.system('cd {} && git stash && git checkout {} && git fetch --tags'.format(engine_path, identifier))


if __name__ == "__main__":
    PARSER = argparse.ArgumentParser(description='PolyEngine project management tool')

    PARSER.add_argument("-e", "--engine", action='store', dest='engine_path',
                        default='..',
                        metavar='engine_path',
                        help='provide custom engine path')
    PARSER.add_argument('-co', '--checkout', action='store_true', dest='checkout',
                        help='TODO help')

    MTX = PARSER.add_mutually_exclusive_group()
    MTX.add_argument('-c', '--create', action=CreateProjectAction, dest='action_to_perform',
                     nargs=2, metavar=('project_path', 'project_name'),
                     help='create project of given name at provided path')
    MTX.add_argument('-u', '--update', action=UpdateProjectAction, dest='action_to_perform',
                     nargs=1, metavar='project_path',
                     help='update project at given path')
    MTX.add_argument('-b', '--bump-version', action=BumpVersionAction, dest='action_to_perform',
                     nargs=1, metavar='project_path',
                     help='sets engine version in game project file to latest')

    ARGS = PARSER.parse_args()

    if ARGS.action_to_perform == ActionType.CREATE:
        create_project(ARGS.project_name, ARGS.project_path, ARGS.engine_path)
    elif ARGS.action_to_perform == ActionType.UPDATE:
        update_project(ARGS.project_path, ARGS.engine_path, ARGS.checkout)
    elif ARGS.action_to_perform == ActionType.BUMP:
        bump_version(ARGS.project_path, ARGS.engine_path)
        update_project(ARGS.project_path, ARGS.engine_path, True)

import os
import glob
import re
import json
import shutil
import fileinput
import xml.etree.ElementTree as ET


import common
from .project_base import ProjectBase, DIST_DIR_NAME

PROJ_FILE_EXTENSION = '.proj.json'
PROJ_FILE_GLOB = '*' + PROJ_FILE_EXTENSION
PROJ_FILE_REGEX = r'.*[\\\/](\S+)' + r'\.'.join(PROJ_FILE_EXTENSION.split('.'))

print(PROJ_FILE_REGEX)

PROJECT_RESOURCES_DIR_NAME = 'Res'
PROJECT_SOURCES_DIR_NAME = 'Src'
GAME_NAME_FILE = 'Game'
GAME_SRC_IN_FILES = [ GAME_NAME_FILE + '.hpp.in', GAME_NAME_FILE + '.cpp.in']
SLN_CMAKE_IN_FILE = 'Sln-CMakeLists.txt.in'
PROJ_CMAKE_IN_FILE = 'Proj-CMakeLists.txt.in'
IN_FILE_REGEX = r'(\S+)\.(\S+)\.in'
CMAKE_LISTS_FILE_NAME = 'CMakeLists.txt'

PROJECT_INIT_RESOURCES_DIR_NAME = 'project-init'
PROJECT_INIT_RESOURCES_PATH = os.path.join(common.SCRIPT_ENV.script_resources_path, PROJECT_INIT_RESOURCES_DIR_NAME)

# Setup:
# $DIST_DIR$ - distribution directory for artifacts copying
# $PROJECT_NAME$ - name of the project
# $PROJECT_PATH$ - relative path of the project (game) directory (relative to root project dir)
# $GAME_CLASS_NAME$ - name of the game class
# $ENGINE_DIR$ - absolute path to engine root directory

class GameProject(ProjectBase):
    def __init__(self, project_root_path, project_name=None, create_if_absent=False, build_postfix=None):
        # Create logger
        self._logger = common.SCRIPT_ENV.get_logger(name='GameProject')

        # Check if root folder exists, raise exception if cannot be created
        create_needed = False
        if not os.path.isdir(project_root_path):
            if not project_name:
                raise ValueError('Cannot create project without project_name provided!')
            create_needed = True

        self._project_name = project_name
        
        # Establish project name
        if not create_needed and not self._project_name:
            project_files = glob.glob(os.path.join(project_root_path, PROJ_FILE_GLOB), recursive=False)
            self._logger.debug('Project name not provided but found following project files: [{}]'.format(project_files))
            if len(project_files) != 1:
                raise FileExistsError('Invalid number of project files [{}] in provided path [{}]. Should be only one.'
                    .format(project_files, project_root_path))

            match = re.match(PROJ_FILE_REGEX, project_files[0])
            if not match:
                raise ValueError('Couldn\'t deduce project name!')
            
            self._project_file_path = os.path.abspath(project_files[0])
            self._parse_proj_file()
            self._logger.info('Deduced project name to be: [{}]'.format(self._project_name))

        # Initialize base
        ProjectBase.__init__(self, project_root_path, self._project_name, build_postfix)

        self._project_path = os.path.join(self._root_path, self._project_name)
        self._project_resources_path = os.path.join(self._project_path, PROJECT_RESOURCES_DIR_NAME)
        self._project_sources_path = os.path.join(self._project_path, PROJECT_SOURCES_DIR_NAME)
        self._project_file_path = os.path.join(self._root_path, self._project_name + PROJ_FILE_EXTENSION)

        if create_needed or not os.path.isfile(self._project_file_path):
            if not create_if_absent:
                raise ValueError('Project [{}] cannot be created when create_if_absent=False.'.format(self._project_name))
            self._create()

        if os.path.isfile(self._project_file_path):
            self._parse_proj_file()

        assert(os.path.isfile(self._project_file_path))
        assert(self._engine_ver is not None)

    def bump_ver(self, version):
        if version <= self._engine_ver:
            raise ValueError('Engine version can only go up! Current: [{}] Requested: [{}]'.format(self._engine_ver, version))
        self._engine_ver = version
        self._save()

    def _create(self):
        # Setup engine version
        self._engine_ver = common.SCRIPT_ENV.engine_ver()

        # Ensure all necessary folders are present
        os.makedirs(self._root_path, exist_ok=True)
        os.makedirs(self._project_path, exist_ok=True)
        os.makedirs(self._project_resources_path, exist_ok=True)
        os.makedirs(self._project_sources_path, exist_ok=True)
        os.makedirs(self._project_resources_path, exist_ok=True)
        os.makedirs(self._project_sources_path, exist_ok=True)

        # Create basic proj file
        self._save_proj_file()

        # Copy all of the source files
        for game_src_file in GAME_SRC_IN_FILES:
            match = re.match(IN_FILE_REGEX, game_src_file)
            if not match:
                raise ValueError('File [{}] does not match regex: [{}]'.format(game_src_file, IN_FILE_REGEX))
            
            game_src_file_path = os.path.join(PROJECT_INIT_RESOURCES_PATH, game_src_file)
            out_src_file_name = self._project_name if match.group(1) == GAME_NAME_FILE else GAME_NAME_FILE 
            out_src_file = '.'.join([out_src_file_name, match.group(2)])
            out_src_file_path = os.path.join(self._project_sources_path, out_src_file)
            
            shutil.copy(game_src_file_path, out_src_file_path)
            self._replace_tags_in_file(out_src_file_path, { '$GAME_CLASS_NAME$':  self._project_name})

    def _before_cmake_gen(self):
        self._update_cmake_files()

    def _after_cmake_gen(self):
        if os.name == 'nt':
            self._patch_vs_proj_file()

    def _save(self):
        self._save_proj_file()

    def _patch_vs_proj_file(self):
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

        usr_proj_path = os.path.join(self._build_path, self._project_name, self._project_name + '.vcxproj.user')
        xml_namespace = { 'ns' : 'http://schemas.microsoft.com/developer/msbuild/2003' }
        namespace_str = '{' + xml_namespace['ns'] + '}'
        ET.register_namespace('', xml_namespace['ns'])
        
        property_group_conditions_to_add = [ "'$(Configuration)|$(Platform)'=='Debug|x64'",
                                    "'$(Configuration)|$(Platform)'=='DebugFast|x64'",
                                    "'$(Configuration)|$(Platform)'=='Release|x64'",
                                    "'$(Configuration)|$(Platform)'=='Testing|x64'" ]

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
            property_group_name = project_property_group.attrib['Condition']
            m = re.search("'\$\(Configuration\)\|\$\(Platform\)'=='(.*)\|(.*)'", property_group_name)
            dbg_config_type = m.group(1) # Release, debug, etc.
            dbg_config_platform = m.group(2) # x64, etc.
            print('Patching property group, type: {}, platform: {}'.format(dbg_config_type, dbg_config_platform))
            
            #if not is_engine_proj:
            dbg_cmd = project_property_group.find('ns:LocalDebuggerCommand', xml_namespace)
            if dbg_cmd == None:
                dbg_cmd = ET.SubElement(project_property_group, namespace_str + 'LocalDebuggerCommand')
            dbg_cmd.text = 'PolyStandalone.exe'
            print('\tset LocalDebuggerCommand to', dbg_cmd.text)

            dbg_args = project_property_group.find('ns:LocalDebuggerCommandArguments', xml_namespace)
            if dbg_args == None:
                dbg_args = ET.SubElement(project_property_group, namespace_str + 'LocalDebuggerCommandArguments')
            dbg_args.text = self._project_file_path + ' ' + dbg_config_type
            print('\tset LocalDebuggerCommandArguments to', dbg_args.text)
            
            dbg_work_dir = project_property_group.find('ns:LocalDebuggerWorkingDirectory', xml_namespace)
            if dbg_work_dir == None:
                dbg_work_dir = ET.SubElement(project_property_group, namespace_str + 'LocalDebuggerWorkingDirectory')
            dbg_work_dir.text = os.path.join(self._dist_path, '$(Configuration)', '')
            print('\tset LocalDebuggerWorkingDirectory to', dbg_work_dir.text)

            dbg_flavor = project_property_group.find('ns:DebuggerFlavor', xml_namespace)
            if dbg_flavor == None:
                dbg_flavor = ET.SubElement(project_property_group, namespace_str + 'DebuggerFlavor')
            dbg_flavor.text = 'WindowsLocalDebugger'
            print('\tset DebuggerFlavor to', dbg_flavor.text)

        xml_indent(root)

        print('Files in {}: {}'.format(self._build_path, os.listdir(self._build_path)))
        p = os.path.join(self._build_path, self._project_name)
        print('Files in {}: {}'.format(p, os.listdir(p)))

        xml_data.write(usr_proj_path, encoding='utf-8', xml_declaration=True)

    def _create_fast_update_script(self):
        script_path = os.path.join(common.SCRIPT_ENV.scripts_path, 'pe_tools.py')
        command = 'update_game_project'

        fileString = 'py {} {} {}'.format(script_path, command, self._as_cmake_path(self._root_path))
        
        if os.name == 'nt':
            fileString = 'py {} {} {}'.format(script_path, command, self._as_cmake_path(self._root_path))
            fileName = os.sep.join([self._root_path, 'quick_update.bat'])
        else:
            fileString = '#!/bin/bash\npython3.6 {} {} {}'.format(script_path, command, self._as_cmake_path(self._root_path))
            fileName = os.sep.join([self._root_path, 'quick_update.sh'])

        with open(fileName, 'w') as outfile:
            outfile.write(fileString)

        if os.name == 'posix':
            os.system("chmod +x {}".format(fileName))

    def _update_cmake_files(self):
        sln_cmake_in_file_path = os.path.join(PROJECT_INIT_RESOURCES_PATH, SLN_CMAKE_IN_FILE)
        proj_cmake_in_file_path = os.path.join(PROJECT_INIT_RESOURCES_PATH, PROJ_CMAKE_IN_FILE)
        sln_cmake_out_file_path = os.path.join(self._root_path, CMAKE_LISTS_FILE_NAME)
        proj_cmake_out_file_path = os.path.join(self._project_path, CMAKE_LISTS_FILE_NAME)

        shutil.copy(sln_cmake_in_file_path, sln_cmake_out_file_path)
        shutil.copy(proj_cmake_in_file_path, proj_cmake_out_file_path)

        # Cmake needs paths with '/' (instead of '\') regardles of platform
        self._replace_tags_in_file(sln_cmake_out_file_path, {'$PROJECT_PATH$': self._project_name, 
                                                        '$PROJECT_NAME$': self._project_name,
                                                        '$DIST_DIR$': DIST_DIR_NAME,
                                                        '$ENGINE_DIR$': self._as_cmake_path(common.SCRIPT_ENV.engine_path) }) 
        self._replace_tags_in_file(proj_cmake_out_file_path, {'$PROJECT_NAME$': self._project_name })

        self._create_fast_update_script()

    def _parse_proj_file(self):
        with open(self._project_file_path, 'r') as file:
            project_file_data = json.load(file, cls=common.serialization.JSONDecoder)
            self._project_name = project_file_data['ProjectName']
            self._engine_ver = project_file_data['EngineVersion']
            
    def _save_proj_file(self):
        project_file_data = {}
        project_file_data['ProjectName'] = self._project_name
        project_file_data['EngineVersion'] = self._engine_ver
        with open(self._project_file_path, 'w') as file:
            json.dump(project_file_data, file, cls=common.serialization.JSONEncoder)

    def _replace_tags_in_file(self, file_to_search, tags_and_values):
        print('Replacing tags in', file_to_search)
        for tag, val in tags_and_values.items():
            print('\tchanging', tag, 'to', val)

        with fileinput.FileInput(file_to_search, inplace=True) as file:
            for line in file:
                for tag, val in tags_and_values.items():
                    line = line.replace(tag, val)
                print(line, end='')

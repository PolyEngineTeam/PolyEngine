import sys
import os
import subprocess
import glob
import logging
import ast
import argparse
import unittest
from collections import namedtuple

# Own imports
import commands.common as common

# Manual constants
LOGGER_FORMAT = '[%(asctime)s] [%(levelname)s] [%(name)s]\t %(message)s'
LOG_LEVEL = 'DEBUG'

RUN_TESTS_CMD = 'run_tests'
RUN_TESTS_DOC = 'Runs all tests.'

TESTS_DIR_NAME = 'tests'
TEST_SCRIPT_NAME_PATTERN = 'test_*.py'

BRIEF_TAG = '@brief'
MISSING_DOCSTRING_MSG = '[missing module docstring]'
MISSING_BRIEF_MSG = '[missing "{}" tag in module docstring]'.format(BRIEF_TAG)

# Automatic constants
SCRIPT_ENV = common.ScriptEnv(os.path.dirname(os.path.realpath(__file__)))

# Global variables
numeric_logging_level = getattr(logging, LOG_LEVEL.upper(), None)
if not isinstance(numeric_logging_level, int):
    raise ValueError('Invalid log level: {}'.format(LOG_LEVEL))

logging.basicConfig(level=numeric_logging_level, format=LOGGER_FORMAT)
logger = logging.getLogger(name='poly_tools')

### SCRIPT START ###
if __name__ != '__main__':
    logger.error('This is a script not a library. Importing it is prohibited.')
    sys.exit(1)

if len(sys.argv) <= 1:
    logger.error('No arguments provided. To display help use -h or --help.')
    sys.exit(1)

logger.info('Script Environment:\n{}'.format(SCRIPT_ENV))

# Gather commands
available_commands = []
CommandTuple = namedtuple('Command', 'name description')
max_command_length = 0

for file in glob.glob(os.path.join(SCRIPT_ENV.commands_path, '*.py'), recursive=False):
    command_name = os.path.splitext(os.path.basename(file))[0]
    command_descripton = MISSING_DOCSTRING_MSG
    max_command_length = max(max_command_length, len(command_name))

    with open(file, 'r') as f:
        tree = ast.parse(f.read())
    docstring = ast.get_docstring(tree)
    if docstring:
        brief = common.first_tagged_line_from_text(docstring, BRIEF_TAG)
        command_descripton = brief if brief else MISSING_BRIEF_MSG

    available_commands.append(CommandTuple(command_name, command_descripton))
available_commands.append(CommandTuple(RUN_TESTS_CMD, RUN_TESTS_DOC))

# Prepare argparse epilog with command names and helps
available_commands_string = 'available commands:\n'
fmt_string = '  - {:<' + str(max_command_length + 2) + '}{}\n'
for cmd in available_commands:
    available_commands_string += fmt_string.format(cmd.name, cmd.description)

# Parse args
parser = argparse.ArgumentParser(description='PolyEngine tools', epilog=available_commands_string, formatter_class=argparse.RawDescriptionHelpFormatter)
command_parser = parser.add_argument_group()
command_parser.add_argument('command_name', action='store',
                    help='Name of the command.', choices=[cmd.name for cmd in available_commands])
command_parser.add_argument('command_args', action='store', help='Arguments passed to the command.', nargs=argparse.REMAINDER)
parsed_args = parser.parse_args()

if parsed_args.command_name == RUN_TESTS_CMD:
    # Run all tests
    tests_path = os.path.join(SCRIPT_ENV.scripts_path, TESTS_DIR_NAME)
    test_files = glob.glob(os.path.join(tests_path, '**', TEST_SCRIPT_NAME_PATTERN), recursive=True)
    test_module_names = [test_file[len(SCRIPT_ENV.scripts_path)+1:len(test_file)-3].replace(os.sep, '.') for test_file in test_files]
    suites = [ unittest.defaultTestLoader.loadTestsFromName(name) for name in test_module_names]
    
    logger.info('Running all tests:')
    logger.debug('List of test modules: {}'.format(test_module_names))
    
    test_suite = unittest.TestSuite(suites)
    unittest.TextTestRunner().run(test_suite)
else:
    script_file_path = os.path.join(SCRIPT_ENV.commands_path, parsed_args.command_name + '.py')
    script_call = [ sys.executable, script_file_path ] + parsed_args.command_args
    env = os.environ.copy()
    env['PYTHONPATH'] = ':'.join(sys.path)
    logger.info('Executing {} with args {}'.format(parsed_args.command_name, parsed_args.command_args))
    subprocess.run(args=script_call, cwd=SCRIPT_ENV.scripts_path, env=env)

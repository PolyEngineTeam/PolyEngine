import sys
import os
import subprocess
import glob
import logging
import ast
import argparse
import unittest
import importlib
import commands
from collections import namedtuple

# Own imports
import common

# Manual constants
RUN_TESTS_CMD = 'run_tests'
RUN_TESTS_DOC = 'Runs all tests.'

TEST_SCRIPT_NAME_PATTERN = 'test_*.py'

BRIEF_TAG = '@brief'
MISSING_DOCSTRING_MSG = '[missing module docstring]'
MISSING_BRIEF_MSG = '[missing "{}" tag in module docstring]'.format(BRIEF_TAG)

# Global variables
logger = common.SCRIPT_ENV.get_logger(name='pe_tools')

### SCRIPT START ###
if __name__ != '__main__':
    logger.error('This is a script not a library. Importing it is prohibited.')
    sys.exit(1)

if len(sys.argv) <= 1:
    logger.error('No arguments provided. To display help use -h or --help.')
    sys.exit(1)

logger.info('Script Environment:\n{}'.format(common.SCRIPT_ENV))

# Gather commands
available_commands = []
CommandTuple = namedtuple('Command', 'name description')
max_command_length = len(RUN_TESTS_CMD)
print(common.SCRIPT_ENV.script_commands_path)
for file in glob.glob(os.path.join(common.SCRIPT_ENV.script_commands_path, '*.py'), recursive=False):
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
    test_files = glob.glob(os.path.join(common.SCRIPT_ENV.script_tests_path, '**', TEST_SCRIPT_NAME_PATTERN), recursive=True)
    test_module_names = [test_file[len(common.SCRIPT_ENV.scripts_path)+1:len(test_file)-3].replace(os.sep, '.') for test_file in test_files]
    suites = [ unittest.defaultTestLoader.loadTestsFromName(name) for name in test_module_names]
    
    logger.info('Running all tests:')
    logger.debug('List of test modules: {}'.format(test_module_names))
    
    test_suite = unittest.TestSuite(suites)
    result = unittest.TextTestRunner().run(test_suite)

    sys.exit(0 if result.wasSuccessful() else 1)
else:
    imported_module = importlib.import_module('commands.' + parsed_args.command_name)
    if hasattr(imported_module, 'execute'):
        logger.info('Executing {} with args {}'.format(parsed_args.command_name, parsed_args.command_args))
        imported_module.execute(*parsed_args.command_args)
    else:
        raise AttributeError('Commands should provide "execute(...)" symbol.')

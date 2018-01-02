import argparse
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
def CreateProject(name, path, enginePath):
    print('Creating project', name, 'in', path, 'with engine at', args.enginePath)

def UpdateProject(path, enginePath):
    print('Updating project at', path, 'with engine at', args.enginePath)

#################### SCRIPT START ####################
parser = argparse.ArgumentParser(description='PolyEngine project management tool')

parser.add_argument("-e", "--engine", action='store', dest='enginePath',
                    default='..',
                    metavar='engine_path',
                    help='provide custom engine path')

mtx = parser.add_mutually_exclusive_group()
mtx.add_argument('-c', '--create', action=CreateProjectAction, dest='actionToPerform',
                    nargs=2, metavar=('project_name', 'project_path'),
                    help='create project of given name at provided path')
mtx.add_argument('-u', '--update', action=UpdateProjectAction, dest='actionToPerform',
                    nargs=1, metavar='project_path',
                    help='update project at given path')


args = parser.parse_args()

if args.actionToPerform == ActionType.CREATE:
    CreateProject(args.project_name, args.project_path, args.enginePath)
elif args.actionToPerform == ActionType.UPDATE:
    UpdateProject(args.project_path, args.enginePath)

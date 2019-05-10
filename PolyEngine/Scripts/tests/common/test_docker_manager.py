import os
import unittest

import common
import common.docker_manager as docker_mgr


class Test(unittest.TestCase):
    def setUp(self):
        import sys
        print(sys.path)
        self.mgr = docker_mgr.DockerManager(dockerfile_dir=os.path.join(common.SCRIPT_ENV.script_resources_path, 'dockerfiles'))

    def tearDown(self):
        pass

    def test_build_image(self):
        img = self.mgr.get_image('ubuntu18', 'gcc8')
        print(img)
        
        try:
            img.run_cmd('echo Hello')
        except:
            self.fail('run_cmd has raised exception!')
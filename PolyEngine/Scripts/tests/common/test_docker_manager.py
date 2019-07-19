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
        try:
            img = self.mgr.get_image('ubuntu18', 'gcc8')
            self.assertIsNotNone(img)
        except:
            self.fail('get_image has raised exception!')

    def test_run_cmd(self):
        TEST_FILE = 'test.tmp'
        self.assertFalse(os.path.isfile(TEST_FILE))
        
        img = self.mgr.get_image('ubuntu18', 'gcc8')

        try:
            img.run_cmd('touch test.tmp')
        except:
            self.fail('run_cmd has raised exception!')

        self.assertTrue(os.path.isfile(TEST_FILE))
        os.remove(TEST_FILE)
        self.assertFalse(os.path.isfile(TEST_FILE))

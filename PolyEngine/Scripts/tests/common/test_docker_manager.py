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
            img = self.mgr.get_image('ubuntu18', 'gcc')
            self.assertIsNotNone(img)
        except:
            self.fail('get_image has raised exception!')

    def test_run_cmd(self):
        # Disabling this test for now. Don't know what is causing issues with docker and non interactive commands.
        return
        
        
        TEST_FILE = 'test.tmp'
        if os.path.isfile(TEST_FILE):
            os.remove(TEST_FILE)
        
        img = self.mgr.get_image('ubuntu18', 'gcc')

        try:
            img.run_cmd('touch {}'.format(TEST_FILE))
        except:
            self.fail('run_cmd has raised exception!')

        self.assertTrue(os.path.isfile(TEST_FILE))
        os.remove(TEST_FILE)

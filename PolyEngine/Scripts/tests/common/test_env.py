import unittest
import os

import common

class Test(unittest.TestCase):
	def setUp(self):
		pass

	def tearDown(self):
		pass

	def test_engine_version(self):
		version = common.SCRIPT_ENV.engine_ver()
		self.assertIsNotNone(version)

	def test_environment_exists(self):
		env = common.SCRIPT_ENV
		
		self.assertTrue(os.path.isdir(env.scripts_path))
		self.assertTrue(os.path.isdir(env.script_commands_path))
		self.assertTrue(os.path.isdir(env.engine_path))
		self.assertTrue(os.path.isdir(env.repo_path))
		self.assertTrue(os.path.isdir(env.script_resources_path))
		self.assertTrue(os.path.isdir(env.script_tests_path))

	def test_env_logger(self):
		TEST_LOGGER_NAME = 'TestLogger'
		logger = common.SCRIPT_ENV.get_logger(TEST_LOGGER_NAME)

		self.assertIsNotNone(logger)
		self.assertEqual(logger.name, TEST_LOGGER_NAME)

	def test_env_os(self):
		is_windows = common.SCRIPT_ENV.is_windows()
		is_linux = common.SCRIPT_ENV.is_linux()
		is_macos = common.SCRIPT_ENV.is_macos()
		is_cygwin = common.SCRIPT_ENV.is_cygwin()
		is_posix = common.SCRIPT_ENV.is_posix()

		if is_windows:
			self.assertFalse(is_linux)
			self.assertFalse(is_macos)
			self.assertFalse(is_cygwin)
			self.assertFalse(is_posix)
		elif is_linux:
			self.assertFalse(is_windows)
			self.assertFalse(is_macos)
			self.assertFalse(is_cygwin)
			self.assertTrue(is_posix)
		elif is_macos:
			self.assertFalse(is_windows)
			self.assertFalse(is_linux)
			self.assertFalse(is_cygwin)
			self.assertTrue(is_posix)
		else:
			self.assertFalse(is_windows)
			self.assertFalse(is_linux)
			self.assertFalse(is_macos)
			self.assertTrue(is_cygwin)
			self.assertTrue(is_posix)
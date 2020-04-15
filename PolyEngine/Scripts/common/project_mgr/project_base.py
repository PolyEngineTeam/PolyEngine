import os
import shutil

BUILD_DIR_NAME = 'Build'
COMMON_BUILD_DIR_NAME = 'CommonBuild'
DIST_DIR_NAME = 'Dist'

class ProjectBase():
    def __init__(self, project_root_path, project_name, build_postfix=None):
        self._root_path = project_root_path

        build_dir_fullname = BUILD_DIR_NAME + '_' + build_postfix if build_postfix else BUILD_DIR_NAME
        dist_dir_fullname = DIST_DIR_NAME + '_' + build_postfix if build_postfix else DIST_DIR_NAME

        self._build_path = os.path.join(project_root_path, build_dir_fullname)
        self._build_postfix = build_postfix
        self._common_build_path = os.path.join(project_root_path, COMMON_BUILD_DIR_NAME)
        self._dist_path = os.path.join(project_root_path, dist_dir_fullname)
        self._project_name = project_name
    

    def update(self):
        self._before_cmake_gen()
        self._run_cmake_generator()
        self._after_cmake_gen()

    def build(self):
        self.update()
        self._build()

    def save(self):
        self._save()

    def clean_build(self):
        shutil.rmtree(self._build_path)
        shutil.rmtree(self._dist_path)
        shutil.rmtree(self._common_build_path)

    @property
    def name(self):
        return self._project_name

    # Abstract API
    def _before_cmake_gen(self):
        pass
    def _after_cmake_gen(self):
        pass
    def _save(self):
        pass

    # Common API
    def _build(self):
        raise NotImplementedError()

    def _as_cmake_path(self, path):
        return path.replace('\\', '/')

    def _run_cmake_generator(self):
        if not os.path.exists(self._build_path):
            os.makedirs(self._build_path)

        cmake_kv_defines = {}
        if self._build_postfix:
            cmake_kv_defines['BUILD_POSTFIX'] = self._build_postfix
        
        cmake_cmd = 'cmake '
        if os.name == 'nt':  # Run cmake update with 64bit arch (using undocumented parameters that work for some reason, src: http://cprieto.com/posts/2016/10/cmake-out-of-source-build.html)
            cmake_cmd += '-A x64 '
        cmake_cmd += '-H{} '.format(self._as_cmake_path(self._root_path))
        cmake_cmd += '-B{} '.format(self._as_cmake_path(self._build_path))

        for k, v in cmake_kv_defines.items():
            cmake_cmd += '-D{}={} '.format(k, v)

        os.system(cmake_cmd)

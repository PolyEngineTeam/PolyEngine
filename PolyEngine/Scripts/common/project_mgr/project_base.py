import os

BUILD_DIR_NAME = 'Build'
DIST_DIR_NAME = 'Dist'

class ProjectBase():
    def __init__(self, project_root_path, project_name, engine_ver, build_dir_name):
        self._root_path = project_root_path
        self._build_path = os.path.join(project_root_path, build_dir_name)
        self._dist_path = os.path.join(project_root_path, DIST_DIR_NAME)
        self._project_name = project_name
        self._engine_ver = engine_ver
    

    def update(self):
        self._before_cmake_gen()
        self._run_cmake_generator()
        self._after_cmake_gen()

    def build(self):
        self.update()
        self._build()

    def save(self):
        self._save()

    @property
    def name(self):
        return self._project_name

    @property
    def engine_ver(self):
        return self._engine_ver

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
        
        # Run cmake update with 64bit arch (using undocumented parameters that work for some reason, src: http://cprieto.com/posts/2016/10/cmake-out-of-source-build.html)
        if os.name == 'nt':
            os.system('cmake -A x64 -H{} -B{}'.format(self._as_cmake_path(self._root_path), self._as_cmake_path(self._build_path)))
        else:
            os.system('cmake -H{} -B{}'.format(self._as_cmake_path(self._root_path), self._as_cmake_path(self._build_path)))

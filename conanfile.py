from conan import ConanFile


class LearnOpenGLQt(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "QbsDeps"

    def requirements(self):
        self.requires("assimp/6.0.5")



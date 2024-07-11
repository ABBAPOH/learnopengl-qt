import qbs

Project {
    OpenGLApplication {
        Depends { name: "cameralib" }
        files: [
            "main.cpp",
            "window.cpp",
            "window.h",
            "resources.qrc",
        ]
    }
    GuiLibrary {
        name: "cameralib"
        files: [
            "camera.cpp",
            "camera.h",
        ]
    }
}
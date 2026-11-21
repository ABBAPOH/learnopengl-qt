import qbs

OpenGLApplication {
    Depends { name: "cameralib" }
    files: [
        "main.cpp",
        "window.cpp",
        "window.h",
        "resources.qrc",
    ]
}

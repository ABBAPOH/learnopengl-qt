import qbs

OpenGLApplication {
    Depends { name: "cameralib" }
    files: [
        "*.cpp",
        "*.h",
        "*.qrc",
    ]
}

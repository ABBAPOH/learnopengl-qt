import qbs

GuiLibrary {
    Depends { name: "Qt.opengl" }
    Depends { name: "assimp" }
    name: "cameralib"
    files: [
        "camera.cpp",
        "camera.h",
        "mesh.cpp",
        "mesh.h",
        "model.h",
        "model.cpp",
        "qtassimpio.h",
        "qtassimpio.cpp",
    ]
}

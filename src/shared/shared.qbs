import qbs

GuiLibrary {
    Depends { name: "Qt.opengl" }
    Depends { name: "assimp" }
    name: "cameralib"
    files: [
        "basewindow.cpp",
        "basewindow.h",
        "camera.cpp",
        "camera.h",
        "camerawindow.cpp",
        "camerawindow.h",
        "mesh.cpp",
        "mesh.h",
        "model.h",
        "model.cpp",
        "qtassimpio.h",
        "qtassimpio.cpp",
    ]
}

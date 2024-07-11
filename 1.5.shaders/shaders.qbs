import qbs

Project {
    minimumQbsVersion: "1.7.1"

    OpenGLApplication {
        files: [
            "*.cpp",
            "*.h",
            "*.qrc",
        ]
    }
}

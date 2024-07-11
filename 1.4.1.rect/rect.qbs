import qbs

Project {
    minimumQbsVersion: "1.7.1"

    OpenGLApplication {
        files: [
            "main.cpp",
            "window.cpp",
            "window.h",
        ]
    }
}

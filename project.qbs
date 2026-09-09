import qbs.Probes

Project {
    name: "LearnOpenGL Qt"
    minimumQbsVersion: "2.6.0"
    qbsSearchPaths: "qbs"
    // Override in CI: qbs resolve project.conanProfile:ci
    property string conanProfile
    property string conanInstallPath: conanProbe.generatedFilesPath
    Probes.ConanfileProbe {
        id: conanProbe
        conanfilePath: project.sourceDirectory + "/conanfile.py"
        generators: "QbsDeps"
        // Assimp has apple-clang 17 prebuilts; some transitive deps only have
        // clang 13 — build those from source when missing.
        additionalArguments: {
            var args = ["--build=missing"];
            if (project.conanProfile)
                args.push("-pr:a=" + project.conanProfile);
            return args;
        }
    }
    references: [
        "src/src.qbs"
    ]
    Product {
        name: "resources"
        files: "resources/**/*"
    }
}

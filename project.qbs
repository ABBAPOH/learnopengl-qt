import qbs.FileInfo
import qbs.Process
import qbs.Probes

Project {
    name: "LearnOpenGL Qt"
    minimumQbsVersion: "2.6.0"
    qbsSearchPaths: "qbs"
    // Override in CI: qbs resolve project.conanProfile:ci
    property string conanProfile
    property string conanInstallPath: conanProbe.generatedFilesPath
    // Conan Center's kuba-zip/0.3.0 checksum no longer matches GitHub's tag
    // archive. Point Conan at the corrected recipe before it resolves assimp.
    Probe {
        id: conanRecipesProbe
        property string recipesDir: project.sourceDirectory + "/conan"
        configure: {
            var conan = "conan" + FileInfo.executableSuffix();
            var p = new Process();
            var code = p.exec(conan, [
                "remote", "add", "learnopengl-recipes", recipesDir,
                "--type=local-recipes-index",
                "--index", "0",
                "--force",
                "--allowed-packages", "kuba-zip/*"
            ], false);
            if (code !== 0) {
                var message = p.readStdErr() || p.readStdOut();
                p.close();
                throw "Failed to register local Conan recipes: " + message;
            }
            p.close();
            found = true;
        }
    }
    Probes.ConanfileProbe {
        id: conanProbe
        conanfilePath: project.sourceDirectory + "/conanfile.py"
        generators: "QbsDeps"
        // Assimp has apple-clang 17 prebuilts; some transitive deps only have
        // clang 13 — build those from source when missing.
        additionalArguments: {
            if (!conanRecipesProbe.found)
                throw "Local Conan recipes were not registered.";
            var args = ["--build=missing", "--update=kuba-zip/*"];
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

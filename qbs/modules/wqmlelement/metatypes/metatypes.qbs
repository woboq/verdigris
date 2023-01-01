import qbs.File

Module {
    Depends { name: "Qt.core" }

    Rule {
        name: "Qt MetaTypes"
        inputs: ["verdigris.metatypes"]
        inputsFromDependencies: ["verdigris.metatypes"]
        Artifact {
            filePath: input.fileName + ".json"
            fileTags: ["qt.core.metatypes"]
            qbs.install: product.Qt.core.metaTypesInstallDir
            qbs.installDir: product.Qt.core.metaTypesInstallDir
        }
        prepare: {
            var cmd = new JavaScriptCommand();
            cmd.description = "generating " + output.fileName + " from " + input.fileName;
            cmd.highlight = "codegen";
            cmd.sourceCode = function() {
                File.copy(input.filePath, output.filePath);
            }
            return [cmd];
        }
    }
}

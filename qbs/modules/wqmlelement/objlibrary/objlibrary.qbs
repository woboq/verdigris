
Module {
    additionalProductTypes: ["obj", "verdigris.metatypes"]

    Depends { productTypes: ["metatypes-extractor"] }

    Rule {
        name: "Extract MetaTypes"
        multiplex: true
        inputs: ["obj"]
        Artifact {
            filePath: product.targetName.toLowerCase() + "_metatypes"
            fileTags: ["verdigris.metatypes"]
        }
        explicitlyDependsOnFromDependencies: ["metatypes-extractor"]
        prepare: {
            var inputFilePaths = inputs.obj.map(function(a) {
                return a.filePath;
            });
            var toolPath = explicitlyDependsOn["metatypes-extractor"][0].filePath;
            var cmd = new Command(toolPath,
                ["--out=" + output.filePath].concat(inputFilePaths));
            cmd.environment = product.environment;
            cmd.description = "generating " + output.fileName;
            cmd.highlight = "codegen";
            return cmd;
        }
    }
}

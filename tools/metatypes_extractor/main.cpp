#include "NtSection.h"
#include "ElfSection.h"

#include <QCoreApplication>
#include <QCommandLineParser>
#include <QStringList>
#include <QFile>
#include <cstdlib>
#include <QJsonArray>
#include <QMap>
#include <QJsonObject>
#include <QJsonDocument>

auto spanContainer(const auto& container) -> BufferSpan {
    return {reinterpret_cast<const uint8_t*>(container.data()), static_cast<size_t>(container.size())};
}
auto toByteArray(const BufferSpan& span) -> QByteArray {
    return QByteArray::fromRawData(reinterpret_cast<const char*>(span.data), static_cast<qsizetype>(span.count));
}

int collectQmlTypes(const QString& outputFilePath, const QStringList& binaryFilePathes) {
    auto outputFile = QFile{outputFilePath};
    if (!outputFile.open(QIODevice::WriteOnly)) {
        fprintf(stderr, "Error opening %s for writing\n", qPrintable(outputFilePath));
        return EXIT_FAILURE;
    }

    const auto qmlTypesString = std::string{"qmltypes"};
    const auto qmlTypesSpan = spanContainer(qmlTypesString);

    auto map = QMap<QString, QJsonObject>{};
    auto sectionCallback = [&](BufferSpan types) {
        types.each_string([&](BufferSpan json) {
            auto jsonError = QJsonParseError{};
            auto jsonDocument = QJsonDocument::fromJson(toByteArray(json));
            if (jsonError.error != QJsonParseError::NoError) {
                fprintf(stderr, "%s at %d\n", jsonError.errorString().toUtf8().constData(), jsonError.offset);
                // return false;
            }
            auto jsonObject = jsonDocument.object();
            auto key = jsonObject.value(QStringLiteral("inputFile")).toString();
            map.insert(key, jsonObject);
        });
    };
    for (auto binaryFilePath : binaryFilePathes) {
        auto binaryFile = QFile{binaryFilePath};
        if (!binaryFile.open(QIODevice::ReadOnly)) {
            fprintf(stderr, "Error opening %s for reading\n", qPrintable(binaryFilePath));
            return EXIT_FAILURE;
        }
        const auto binaryData = binaryFile.readAll();
        const auto binarySpan = spanContainer(binaryData);

        const bool isSuccess = extract_pe_section(binarySpan, qmlTypesSpan, sectionCallback) ||
            extract_elf_section(binarySpan, qmlTypesSpan, sectionCallback) ||
            extract_coff_section(binarySpan, qmlTypesSpan, sectionCallback);
        if (!isSuccess) {
            // fprintf(stderr, "Error finding qmltypes section in %s\n", qPrintable(binaryFilePath));
            // return EXIT_FAILURE;
        }
    }

    auto jsonArray = QJsonArray{};
    const auto jsonValues = map.values();
    for (auto value : jsonValues) {
        jsonArray.append(value);
    }

    const auto outputDocument = QJsonDocument{jsonArray};
    outputFile.write(outputDocument.toJson());

    return EXIT_SUCCESS;
}

int main(int argc, char* argv[]) {
    auto app = QCoreApplication{argc, argv};
    QCoreApplication::setApplicationVersion(QStringLiteral("1.0.0"));

    auto cmdParser = QCommandLineParser{};
    cmdParser.addHelpOption();
    cmdParser.addVersionOption();

    auto outputOption = QCommandLineOption{
        QStringLiteral("out"), QStringLiteral("File path the output json is stored at."), QStringLiteral("file")};
    cmdParser.addOption(outputOption);

    cmdParser.addPositionalArgument(
        QStringLiteral("[binary-file]"), QStringLiteral("File pathes of compiled object files"));

    cmdParser.process(app.arguments());

    const auto binaryFilePathes = cmdParser.positionalArguments();
    const auto outputFilePath = cmdParser.value(outputOption);

    return collectQmlTypes(outputFilePath, binaryFilePathes);
}

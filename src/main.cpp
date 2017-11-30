#include <QCoreApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QProcess>
#include <QFile>

#include <iostream>
#include <memory>

#include "ObjDumpParser.h"
#include "ScParser.h"
#include "ScModel.h"

#include "StmListGenerator.h"
#include "TextGenerator.h"
#include "PlantUmlGenerator.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("bosce"); // BOost State Chart Extractor

    QCommandLineParser parser;
    parser.setApplicationDescription("Boost statechart extractor from binary objdump");
    parser.addHelpOption();
    parser.addPositionalArgument("input", "Input binary or objdump");
    parser.addOptions({
        { {"O", "objdump"}, "An input file is an objdump instead of binary" },
        { {"X", "extract"}, "Only extract an objdump out of an input binary" },
        { {"l", "list"}, "List all state-machine names" },
        { {"t", "highlight"},
          "Highlight given names in the generated state-machine output (multiple use possible)", "highlight" },
        { {"s", "stm"}, "Generate an output for a specific state-machine", "state-machine" },
        { {"g", "generator"}, "Specify a generator (plantuml, stmlist, text)", "generator" }
    });
    parser.process(app);

    // Arguments:
    const QStringList args = parser.positionalArguments();
    if ( args.isEmpty() ) {
        std::cerr << "An input file is expected\n" << std::endl;
        return -1;
    }
    if ( args.size() > 1 ) {
        std::cerr << "Several input files are specified\n" << std::endl;
        return -1;
    }
    const QString &inputFile = args.at(0);
    const QString stmName = parser.value("stm");
    QString generatorName = parser.value("generator");
    const QStringList highlightNameList = parser.values("highlight");

    const bool isUseObjdump = parser.isSet("objdump");
    const bool isCmdOnlyExtractObjdump = parser.isSet("extract");
    const bool isCmdList = parser.isSet("list");

    if ( isCmdOnlyExtractObjdump ) {
        if ( isUseObjdump || isCmdList || !stmName.isEmpty() || !generatorName.isEmpty() ) {
            std::cerr << "-X option cannot be combined with other options\n" << std::endl;
            return -1;
        }
    } else if ( isCmdList ) {
        if ( !stmName.isEmpty() || !generatorName.isEmpty() ) {
            std::cerr << "-l option cannot be combined with -s and -g options\n" << std::endl;
            return -1;
        }
        generatorName = "stmlist";
    } else if ( stmName.isEmpty() && !(generatorName == "stmlist" || generatorName == "text") ) {
        std::cerr << "A state-machine name must be specified\n" << std::endl;
        return -1;
    } else if ( generatorName.isEmpty() ) {
        // by default use plantuml as the most sophisticated generator:
        generatorName = "plantuml";
    }

    // Model:
    ScModel model;

    // Highlight name set:
    ScNameSet highlightSet;
    for( const QString &highlightName : highlightNameList )
    {
        highlightSet.insert(highlightName.toLocal8Bit());
    }

    // Generator:
    std::unique_ptr<AbstractGenerator> generator;
    if ( isCmdOnlyExtractObjdump ) {
        /* do nothing */
    } else if ( generatorName == "plantuml" ) {
        generator = std::make_unique<PlantUmlGenerator>(model, highlightSet);
    } else if ( generatorName == "stmlist" ) {
        generator = std::make_unique<StmListGenerator>(model);
    } else if ( generatorName == "text" ) {
        generator = std::make_unique<TextGenerator>(model, highlightSet);
    } else {
        std::cerr << "Invalid generator: " << qPrintable(generatorName) << "\n";
        std::cerr << "Available generators: plantuml (default), stmlist and text\n" << std::endl;
        return -1;
    }

    // Parsing:
    ScParser scParser(model);
    ObjDumpParser objDumpParser(scParser);

    if ( isUseObjdump ) {
        /* Use a pre-generated objdump file */
        QFile file(inputFile);
        if ( !file.open(QIODevice::ReadOnly | QIODevice::Text) ) {
            std::cerr << "Cannot open an input file\n" << std::endl;
            return -1;
        }
        objDumpParser.parse(file);

    } else {
        /* Generate an objfile on the fly. */
        QProcess process;

        if ( isCmdOnlyExtractObjdump ) {
            process.setProcessChannelMode(QProcess::ForwardedChannels);
        } else {
            process.setProcessChannelMode(QProcess::ForwardedErrorChannel);
        }

        process.start("objdump", {"-j", ".text", "-C", "-d", inputFile});
        process.waitForFinished();

        if ( (process.exitStatus() != QProcess::NormalExit) || process.exitCode() ) {
            std::cerr << "Cannot generate an objdump\n" << std::endl;
        }

        if ( !isCmdOnlyExtractObjdump ) {
            objDumpParser.parse(process);
        } else {
            return 0;
        }
    }

    // Generation:
    QFile stdout;
    stdout.open(1, QIODevice::WriteOnly | QIODevice::Text);
    generator->generate(stdout, stmName.toLocal8Bit());

    return 0;
}

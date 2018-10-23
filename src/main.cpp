/*
 * Boost StateChart Extractor
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE or copy at
 *                      http://www.boost.org/LICENSE_1_0.txt)
 */

#include "ObjDumpParser.h"
#include "ScParser.h"
#include "ScModel.h"

#include "StmListGenerator.h"
#include "TextGenerator.h"
#include "PlantUmlGenerator.h"

#include <QProcess>
#include <QFile>


#include <boost/program_options.hpp>

#include <iostream>
#include <memory>

namespace po = boost::program_options;

struct Settings
{
    std::string inputFile;
    std::string stmName;
    std::string generatorName;
    std::vector<std::string> highlightNameList;
    bool isUseObjdump;
    bool isExtractObjdump;
    bool isListStms;
};

static bool parseOptions(Settings &settings, int argc, char *argv[])
{
    po::options_description oVisible("Generic options");
    oVisible.add_options()
            ("help,h", "Show this help message")
            ("objdump,O", "An input file is an objdump instead of binary")
            ("extract,X", "Only extract an objdump out of an input binary")
            ("list,l", "List all state-machine names")
            ("highlight,t", po::value<std::vector<std::string>>(&settings.highlightNameList),
             "Highlight given names in the generated state-machine output (multiple use possible)")
            ("stm,s", po::value<std::string>(&settings.stmName),
             "Generate an output for a specific state-machine")
            ("generator,g", po::value<std::string>(&settings.generatorName),
             "Specify a generator (plantuml, stmlist, text)");

    po::options_description oHidden;
    oHidden.add_options()
            ("input", po::value<std::string>(&settings.inputFile), "Input file");

    po::positional_options_description poDesc;
    poDesc.add("input", -1);

    po::options_description oDesc;
    oDesc.add(oHidden).add(oVisible);

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv)
              .options(oDesc).positional(poDesc).run(), vm);
    po::notify(vm);

    auto isSpecified = [&vm](const char *optName)
    {
        return vm.count(optName) > 0;
    };

    if ( isSpecified("help") ) {
        std::cout << "bosce: BOost State Chart Extractor (from objdump)\n" << std::endl;
        std::cout << oVisible << std::endl;
        return false;
    }

    settings.isUseObjdump = isSpecified("objdump");
    settings.isExtractObjdump = isSpecified("extract");
    settings.isListStms = isSpecified("list");

    if ( settings.isExtractObjdump ) {
        if ( settings.isUseObjdump || settings.isListStms ||
             !settings.stmName.empty() || !settings.generatorName.empty() ) {
            throw po::error("option '-X' cannot be combined with other options");
        }
    } else if ( settings.isListStms ) {
        if ( !settings.stmName.empty() || !settings.generatorName.empty() ) {
            throw po::error("option '-l' cannot be combined with options '-s' and '-g'");
        }
        settings.generatorName = "stmlist";
    } else if ( settings.generatorName.empty() ) {
        // by default use plantuml as the most sophisticated generator:
        settings.generatorName = "plantuml";
    }

    return true;
}

int main(int argc, char *argv[])
{
    // Command line arguments:
    Settings settings;

    try {
        if (!parseOptions(settings, argc, argv)) {
            return 0;
        }
    } catch (const po::error &err) {
        std::cerr << "Error: " << err.what() << std::endl;
        return -1;
    }

    // Model:
    ScModel model;

    // Highlight name set:
    ScNameSet highlightSet;
    for( const auto &highlightName : settings.highlightNameList )
    {
        highlightSet.insert(ScName::fromStdString(highlightName));
    }

    // Generator:
    std::unique_ptr<AbstractGenerator> generator;
    if ( settings.isExtractObjdump ) {
        /* do nothing */
    } else if ( settings.generatorName == "plantuml" ) {
        generator = std::make_unique<PlantUmlGenerator>(model, highlightSet);
    } else if ( settings.generatorName == "stmlist" ) {
        generator = std::make_unique<StmListGenerator>(model);
    } else if ( settings.generatorName == "text" ) {
        generator = std::make_unique<TextGenerator>(model, highlightSet);
    } else {
        std::cerr << "Invalid generator: " << settings.generatorName << "\n";
        std::cerr << "Available generators: plantuml (default), stmlist and text\n" << std::endl;
        return -1;
    }

    // Parsing:
    ScParser scParser(model);
    ObjDumpParser objDumpParser(scParser);

    if ( settings.isUseObjdump ) {
        /* Use a pre-generated objdump file */
        QFile file(QString::fromStdString(settings.inputFile));
        if ( !file.open(QIODevice::ReadOnly | QIODevice::Text) ) {
            std::cerr << "Cannot open an input file\n" << std::endl;
            return -1;
        }
        objDumpParser.parse(file);

    } else {
        /* Generate an objfile on the fly. */
        QProcess process;

        if ( settings.isExtractObjdump ) {
            process.setProcessChannelMode(QProcess::ForwardedChannels);
        } else {
            process.setProcessChannelMode(QProcess::ForwardedErrorChannel);
        }

        process.start("objdump", {"-j", ".text", "-C", "-d",
                                  QString::fromStdString(settings.inputFile)});
        process.waitForFinished(-1);

        if ( (process.exitStatus() != QProcess::NormalExit) || process.exitCode() ) {
            std::cerr << "Cannot generate an objdump\n" << std::endl;
        }

        if ( !settings.isExtractObjdump ) {
            objDumpParser.parse(process);
        } else {
            return 0;
        }
    }

    // Generation:
    QFile stdout;
    stdout.open(1, QIODevice::WriteOnly | QIODevice::Text);
    generator->generate(stdout, ScName::fromStdString(settings.stmName));

    return 0;
}

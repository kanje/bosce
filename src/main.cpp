/*
 * Boost StateChart Extractor
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE or copy at
 *                      http://www.boost.org/LICENSE_1_0.txt)
 */

#include "ObjDumpParser.h"
#include "ScModel.h"
#include "ScParser.h"

#include "PlantUmlGenerator.h"
#include "StmListGenerator.h"
#include "TextGenerator.h"
#include "YamlGenerator.h"

#include <boost/process.hpp>
#include <boost/program_options.hpp>
#include <iostream>
#include <memory>

namespace po = boost::program_options;
namespace bp = boost::process;

struct Settings
{
    std::string inputFile;
    std::string stmName;
    std::string generatorName;
    std::vector<std::string> highlightNameList;
    bool isUseObjdump;
    bool isExtractObjdump;
    bool isStripObjdump;
    bool isListStms;
};

static bool parseOptions(Settings &settings, int argc, char *argv[])
{
    po::options_description oVisible("Generic options");
    // clang-format off
    oVisible.add_options()
            ("help,h", "Show this help message")
            ("objdump,O", "An input file is an objdump instead of binary")
            ("extract,X", "Only extract an objdump out of an input binary")
            ("strip,S", "Strip an objdump to contain only relevat data")
            ("list,l", "List all state-machine names")
            ("highlight,t", po::value<std::vector<std::string>>(&settings.highlightNameList),
             "Highlight given names in the generated state-machine output (multiple use possible)")
            ("stm,s", po::value<std::string>(&settings.stmName),
             "Generate an output for a specific state-machine")
            ("generator,g", po::value<std::string>(&settings.generatorName),
             "Specify a generator (plantuml, stmlist, text, yaml)");
    // clang-format on

    po::options_description oHidden;
    oHidden.add_options()("input", po::value<std::string>(&settings.inputFile), "Input file");

    po::positional_options_description poDesc;
    poDesc.add("input", -1);

    po::options_description oDesc;
    oDesc.add(oHidden).add(oVisible);

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(oDesc).positional(poDesc).run(), vm);
    po::notify(vm);

    auto isSpecified = [&vm](const char *optName) { return vm.count(optName) > 0; };

    if (isSpecified("help")) {
        std::cout << "bosce: BOost State Chart Extractor (from objdump)\n\n"
                     "Usage:\n  bosce <input file> [options]\n\n"
                  << oVisible << std::endl;
        return false;
    }

    if (!isSpecified("input")) {
        throw po::error("input file is not specified");
    }

    settings.isUseObjdump = isSpecified("objdump");
    settings.isExtractObjdump = isSpecified("extract");
    settings.isStripObjdump = isSpecified("strip");
    settings.isListStms = isSpecified("list");

    if (settings.isExtractObjdump) {
        if (settings.isUseObjdump || settings.isListStms || !settings.stmName.empty()
            || !settings.generatorName.empty()) {
            throw po::error("option '-X' can be combined only with option '-S'");
        }
    } else if (settings.isStripObjdump) {
        if (!(settings.isUseObjdump || settings.isExtractObjdump)) {
            throw po::error("option '-S' must be combined with either '-X' or '-O'");
        }
        if (settings.isListStms || !settings.stmName.empty() || !settings.generatorName.empty()) {
            throw po::error("option '-S' can be combined only with option '-X'");
        }
    } else if (settings.isListStms) {
        if (!settings.stmName.empty() || !settings.generatorName.empty()) {
            throw po::error("option '-l' cannot be combined with options '-s' and '-g'");
        }
        settings.generatorName = "stmlist";
    } else if (settings.generatorName.empty()) {
        // by default use plantuml as the most sophisticated generator:
        settings.generatorName = "plantuml";
    }

    return true;
}

int main(int argc, char *argv[])
try {
    // Command line arguments:
    Settings settings;
    if (!parseOptions(settings, argc, argv)) {
        return EXIT_SUCCESS;
    }

    // Model:
    ScModel model;

    // Highlight name set:
    ScNameSet highlightSet;
    for (const auto &highlightName : settings.highlightNameList) {
        highlightSet.insert(highlightName);
    }

    // Generator:
    std::unique_ptr<AbstractGenerator> generator;
    if (settings.isExtractObjdump || settings.isStripObjdump) {
        /* do nothing */
    } else if (settings.generatorName == "plantuml") {
        generator = std::make_unique<PlantUmlGenerator>(model, highlightSet);
    } else if (settings.generatorName == "stmlist") {
        generator = std::make_unique<StmListGenerator>(model);
    } else if (settings.generatorName == "text") {
        generator = std::make_unique<TextGenerator>(model, highlightSet);
    } else if (settings.generatorName == "yaml") {
        generator = std::make_unique<YamlGenerator>(model, highlightSet);
    } else {
        std::cerr << "Invalid generator: " << settings.generatorName << "\n";
        std::cerr << "Available generators: plantuml (default), stmlist, text and yaml\n"
                  << std::endl;
        return -1;
    }

    // Parsing:
    ScParser scParser(model);
    ObjDumpParser objDumpParser(scParser);

    if (settings.isUseObjdump) {
        /* Use a pre-generated objdump file */
        std::ifstream file(settings.inputFile);
        if (!file) {
            throw std::runtime_error("Cannot open an input file");
        }
        objDumpParser.parse(file, settings.isStripObjdump);

    } else {
        /* Generate an objfile on the fly. */
        const auto isFullExtractMode = settings.isExtractObjdump && !settings.isStripObjdump;
        bp::ipstream stream;

        auto objdump = [&](auto &output) {
            return bp::child(bp::search_path("objdump"), "-j", ".text", "-C", "-d",
                             settings.inputFile, bp::std_out > output, bp::std_err > stderr);
        };

        auto child = isFullExtractMode ? objdump(stdout) : objdump(stream);
        if (!isFullExtractMode) {
            objDumpParser.parse(stream, settings.isStripObjdump);
        }

        child.wait();
        if (child.exit_code() != 0) {
            throw std::runtime_error("objdump returned " + std::to_string(child.exit_code())
                                     + "; invalid input binary name?");
        }
    }

    // Do not generate the output if an objdump strip is requested:
    if (settings.isExtractObjdump || settings.isStripObjdump) {
        return 0;
    }

    // Generation:
    generator->generate(std::cout, settings.stmName);

    return 0;
} catch (const std::exception &ex) {
    std::cerr << "Error: " << ex.what() << std::endl;
    return EXIT_FAILURE;
}

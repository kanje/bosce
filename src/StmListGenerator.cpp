/*
 * Boost StateChart Extractor
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE or copy at
 *                      http://www.boost.org/LICENSE_1_0.txt)
 */

#include "StmListGenerator.h"

void StmListGenerator::generate(std::ostream &output, const ScName &stmName)
{
    const auto &rootState = m_model.states().at(ScModel::RootScName);

    if (!stmName.empty()) {
        output << "Note: discarding a state-machine name\n";
    }

    if (rootState.substates.empty()) {
        output << "No state-machines found\n";
    } else {
        output << "Available state-machines:\n";
        for (const auto &stm : rootState.substates[0].states) {
            output << "    " << stm << "\n";
        }
    }

    output.flush();
}

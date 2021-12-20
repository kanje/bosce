/*
 * Boost StateChart Extractor
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE or copy at
 *                      http://www.boost.org/LICENSE_1_0.txt)
 */

#include "TextGenerator.h"

ScName TextGenerator::hlText(const ScName &name) const
{
    if (isHighlighted(name)) {
        return "*" + name + "*";
    }
    return name;
}

void TextGenerator::generate(std::ostream &output, const ScName &stmName)
{
    const auto &states = m_model.states();

    if (!stmName.empty()) {
        output << "Note: discarding a state-machine name\n";
    }

    for (const auto &[name, state] : states) {
        output << "Name: " << hlText(name) << "\n";
        output << "Parent: " << hlText(state.parent) << "\n";
        for (ScRegionNum i = 0; i < state.regions.size(); i++) {
            output << "Substates (" << i << ") [" << state.regions[i].initial << "]:\n";
            for (const auto &substate : state.regions[i].states) {
                output << "    " << hlText(substate) << "\n";
            }
        }
        if (!state.transitions.empty()) {
            output << "Transitions:\n";
            for (const auto &[target, events] : state.transitions) {
                output << "==> " << hlText(target) << "\n";
                for (const auto &event : events) {
                    output << "    ** " << hlText(event) << "\n";
                }
            }
        }
        if (!state.deferrals.empty()) {
            output << "Deferrals:\n";
            for (const auto &event : state.deferrals) {
                output << "    " << hlText(event) << "\n";
            }
        }
        output << "\n";
    }

    output.flush();
}

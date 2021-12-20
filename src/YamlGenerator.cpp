/*
 * Boost StateChart Extractor
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE or copy at
 *                      http://www.boost.org/LICENSE_1_0.txt)
 */

#include "YamlGenerator.h"

static void print(std::ostream &output, const ScHistoryState &historyState) noexcept
{
    if (historyState.mode != ScHistoryMode::None) {
        output << "  history:\n";
        output << "    mode: " << toString(historyState.mode) << "\n";
        output << "    initial: " << historyState.initial << "\n";
    }
}

static void print(std::ostream &output, const ScRegionList &regions) noexcept
{
    if (regions.size() > 0) {
        std::string indent;
        if (regions.size() > 1) {
            output << "  regions:\n";
            indent = "  ";
        }
        for (const auto &region : regions) {
            output << indent << "  initial: " << region.initial << "\n";
            output << indent << "  substates:\n";
            for (const auto &substate : region.states) {
                output << indent << "    - " << substate << "\n";
            }
        }
    }
}

static void print(std::ostream &output, const ScTransitionMap &transitions) noexcept
{
    if (!transitions.empty()) {
        output << "  transitions:\n";
        for (const auto &[target, events] : transitions) {
            output << "    - target: " << target.name << "\n";
            if (target.historyMode != ScHistoryMode::None) {
                output << "      history: " << toString(target.historyMode) << "\n";
            }
            output << "      events:\n";
            for (const auto &event : events) {
                output << "        - " << event << "\n";
            }
        }
    }
}

static void print(std::ostream &output, const ScNameSet &deferrals) noexcept
{
    if (!deferrals.empty()) {
        output << "  deferrals:\n";
        for (const auto &event : deferrals) {
            output << "    - " << event << "\n";
        }
    }
}

void YamlGenerator::generate(std::ostream &output, const ScName &stmName)
{
    const auto &states = m_model.states();

    if (!stmName.empty()) {
        output << "Note: discarding a state-machine name\n";
    }

    for (const auto &[name, state] : states) {
        output << name << ":\n";
        if (state.isDefined()) {
            output << "  parent: " << state.parent << "\n";
        }

        print(output, state.historyState);
        print(output, state.regions);
        print(output, state.transitions);
        print(output, state.deferrals);

        output << "\n";
    }

    output.flush();
}

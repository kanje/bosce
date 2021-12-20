/*
 * Boost StateChart Extractor
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE or copy at
 *                      http://www.boost.org/LICENSE_1_0.txt)
 */

#include "PlantUmlGenerator.h"

#include <iomanip>

static ScName removePrefix(const ScName &name)
{
    int nrAngles = 0;
    const char *data = name.data();
    const char *res = data;

    while (*data) {
        switch (*data) {
        case ':':
            if (nrAngles == 0)
                res = data + 1;
            break;

        case '<':
            nrAngles++;
            break;

        case '>':
            nrAngles--;
            break;
        }
        data++;
    }

    return res;
}

static const char *puHistoryState(ScHistoryMode historyMode) noexcept
{
    switch (historyMode) {
    case ScHistoryMode::Deep:
    case ScHistoryMode::Full:
        return "[H*]";
        break;
    case ScHistoryMode::Shallow:
        return "[H]";
        break;
    default:
        break;
    }
    return "";
}

void PlantUmlGenerator::generate(std::ostream &output, const ScName &stmName)
{
    output << "@startuml\n";
    addWelcomeNote(output, stmName);
    generate(output, stmName, 0);
    output << "@enduml\n";
    output.flush();
}

void PlantUmlGenerator::addWelcomeNote(std::ostream &output, const ScName &stmName)
{
    const auto time = std::time(nullptr);
    output << "note \"" << stmName << "\\nGenerated on "
           << std::put_time(std::localtime(&time), "%d.%m.%Y") << "\" as NoteGenerated #A2F2A2\n";
}

void PlantUmlGenerator::generate(std::ostream &output, const ScName &name, int indentLevel)
{
    const auto &state = m_model.states().at(name);
    const auto &nameAlias = alias(name);

    std::string indent(indentLevel * 2, ' ');

    // Exclude the outerbox with a state machine name:
    if (indentLevel > 0) {
        beginState(output, indent, name, nameAlias);
    }

    // History pseudo-state:
    if (auto historyMode = state.historyState.mode; historyMode != ScHistoryMode::None) {
        historyPseudoState(output, indent, alias(state.historyState.initial), historyMode);
    }

    // Substates:
    const auto totalRegions = state.regions.size();
    for (ScRegionNum regionNum = 0; regionNum < totalRegions; regionNum++) {
        if (regionNum > 0) {
            output << indent << "--\n";
        }

        const auto &substates = state.regions[regionNum];
        output << indent << "[*] --> " << alias(substates.initial) << "\n";
        for (const auto &substateName : substates.states) {
            generate(output, substateName, indentLevel + 1);
        }
    }

    if (indentLevel > 0) {
        endState(output, indent);
    }

    // Deferrals:
    if (!state.deferrals.empty()) {
        output << indent << nameAlias << " : Deferrals:\n";
        for (const auto &eventName : state.deferrals) {
            deferralLink(output, indent, nameAlias, eventName);
        }
    }

    // Transitions:
    for (const auto &[target, eventNames] : state.transitions) {
        bool isTransitionHighlighted = false;
        for (auto &eventName : eventNames) {
            if (!eventName.empty() && isHighlighted(eventName)) {
                isTransitionHighlighted = true;
            }
        }

        auto targetAlias = alias(target.name);

        // History states in PlantUML can be references as StateName[H] or StateName[H*]:
        if (target.historyMode != ScHistoryMode::None) {
            targetAlias.append(puHistoryState(target.historyMode));
        }

        transitionLink(output, indent, isTransitionHighlighted, nameAlias, targetAlias, eventNames);
    }
}

const ScName &PlantUmlGenerator::alias(const ScName &name)
{
    if (m_alias.find(name) == m_alias.cend()) {
        m_alias.insert({name, std::to_string(m_nextAlias++)});
    }
    return m_alias[name];
}

void PlantUmlGenerator::beginState(std::ostream &output, std::string &indent, const ScName &name,
                                   const ScName &nameAlias)
{
    const ScName nameShort = removePrefix(name);

    if (isHighlighted(name)) {
        output << indent << "state \"" << nameShort << "\" as " << nameAlias << " #IndianRed {\n";
    } else {
        output << indent << "state \"" << nameShort << "\" as " << nameAlias << " {\n";
    }

    indent.append("  ");
}

void PlantUmlGenerator::endState(std::ostream &output, std::string &indent)
{
    indent.erase(indent.size() - 2);
    output << indent << "}\n";
}

void PlantUmlGenerator::historyPseudoState(std::ostream &output, const std::string &indent,
                                           const ScName &targetAlias, ScHistoryMode historyMode)
{
    output << indent << puHistoryState(historyMode) << " --> " << targetAlias << "\n";
}

void PlantUmlGenerator::deferralLink(std::ostream &output, std::string &indent,
                                     const ScName &stateAlias, const ScName &eventName)
{
    const ScName eventNameShort = removePrefix(eventName);

    if (isHighlighted(eventName)) {
        output << indent << stateAlias << " : * <b>" << eventNameShort << "</b>\n";
    } else {
        output << indent << stateAlias << " : * " << eventNameShort << "\n";
    }
}

void PlantUmlGenerator::transitionLink(std::ostream &output, std::string &indent,
                                       bool isTransitionHighlighted, const ScName &stateAliasOrigin,
                                       const ScName &stateAliasTarget, const ScNameSet &eventNames)
{

    if (isTransitionHighlighted) {
        output << indent << stateAliasOrigin << "-[#red,bold]-->" << stateAliasTarget << " : ";
    } else {
        output << indent << stateAliasOrigin << "-->" << stateAliasTarget << " : ";
    }

    for (auto eventName : eventNames) {
        if (eventName.empty()) {
            eventName = "<unknown>";
        }

        if (isHighlighted(eventName)) {
            output << "<b>" << removePrefix(eventName) << "</b>\\n";
        } else {
            output << removePrefix(eventName) << "\\n";
        }
    }

    output << "\n";
}

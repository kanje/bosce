#include "PlantUmlGenerator.h"

#include <QDateTime>
#include <QIODevice>
#include <QTextStream>

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

void PlantUmlGenerator::generate(QIODevice &output, const ScName &stmName)
{
    QTextStream cout(&output);

    cout << "@startuml\n";
    addWelcomeNote(cout, stmName);
    generate(cout, stmName, 0);
    cout << "@enduml\n";
    cout.flush();
}

void PlantUmlGenerator::addWelcomeNote(QTextStream &cout, const ScName &stmName)
{
    cout << "note \"" << stmName << "\\nGenerated on "
         << QDateTime::currentDateTime().toString("dd.MM.yyyy") << "\" as NoteGenerated #A2F2A2\n";
}

void PlantUmlGenerator::generate(QTextStream &cout, const ScName &name, int indentLevel)
{
    const auto &state = m_model.states()[name];
    const auto &nameAlias = alias(name);

    QByteArray indent(indentLevel * 2, ' ');

    // Exclude the outerbox with a state machine name:
    if (indentLevel > 0) {
        beginState(cout, indent, name, nameAlias);
    }

    // Substates:
    const auto nrOrthRegions = state.substates.size();
    for (auto orthRegion = 0; orthRegion < nrOrthRegions; orthRegion++) {
        if (orthRegion > 0) {
            cout << indent << "--\n";
        }

        const auto &substates = state.substates[orthRegion];
        cout << indent << "[*] --> " << alias(substates.initial) << "\n";
        for (const auto &substateName : substates.states) {
            generate(cout, substateName, indentLevel + 1);
        }
    }

    if (indentLevel > 0) {
        endState(cout, indent);
    }

    // Deferrals:
    if (!state.deferrals.empty()) {
        cout << indent << nameAlias << " : Deferrals:\n";
        for (const auto &eventName : state.deferrals) {
            deferralLink(cout, indent, nameAlias, eventName);
        }
    }

    // Transitions:
    const auto targetNames = state.transitions.keys();
    for (const auto &targetName : targetNames) {
        bool isTransitionHighlighted = false;
        const auto &eventNames = state.transitions[targetName];

        for (auto &eventName : eventNames) {
            if (!eventName.isEmpty() && isHighlighted(eventName)) {
                isTransitionHighlighted = true;
            }
        }

        transitionLink(cout, indent, isTransitionHighlighted, nameAlias, alias(targetName),
                       eventNames);
    }
}

const ScName &PlantUmlGenerator::alias(const ScName &name)
{
    if (!m_alias.contains(name)) {
        m_alias.insert(name, ScName::number(m_nextAlias++));
    }
    return m_alias[name];
}

void PlantUmlGenerator::beginState(QTextStream &cout, QByteArray &indent, const ScName &name,
                                   const ScName &nameAlias)
{
    const ScName nameShort = removePrefix(name);

    if (isHighlighted(name)) {
        cout << indent << "state \"" << nameShort << "\" as " << nameAlias << " #IndianRed {\n";
    } else {
        cout << indent << "state \"" << nameShort << "\" as " << nameAlias << " {\n";
    }

    indent.append("  ");
}

void PlantUmlGenerator::endState(QTextStream &cout, QByteArray &indent)
{
    indent.chop(2);
    cout << indent << "}\n";
}

void PlantUmlGenerator::deferralLink(QTextStream &cout, QByteArray &indent,
                                     const ScName &stateAlias, const ScName &eventName)
{
    const ScName eventNameShort = removePrefix(eventName);

    if (isHighlighted(eventName)) {
        cout << indent << stateAlias << " : * <b>" << eventNameShort << "</b>\n";
    } else {
        cout << indent << stateAlias << " : * " << eventNameShort << "\n";
    }
}

void PlantUmlGenerator::transitionLink(QTextStream &cout, QByteArray &indent,
                                       bool isTransitionHighlighted, const ScName &stateAliasOrigin,
                                       const ScName &stateAliasTarget, const ScNameSet &eventNames)
{

    if (isTransitionHighlighted) {
        cout << indent << stateAliasOrigin << "-[#red,bold]-->" << stateAliasTarget << " : ";
    } else {
        cout << indent << stateAliasOrigin << "-->" << stateAliasTarget << " : ";
    }

    for (auto eventName : eventNames) {
        if (eventName.isEmpty()) {
            eventName = "<unknown>";
        }

        if (isHighlighted(eventName)) {
            cout << "<b>" << removePrefix(eventName) << "</b>\\n";
        } else {
            cout << removePrefix(eventName) << "\\n";
        }
    }

    cout << "\n";
}

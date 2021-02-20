#include "TextGenerator.h"

#include <QIODevice>
#include <QTextStream>

ScName TextGenerator::hlText(const ScName &name) const
{
    if (isHighlighted(name)) {
        return "*" + name + "*";
    }
    return name;
}

void TextGenerator::generate(QIODevice &output, const ScName &stmName)
{
    QTextStream cout(&output);
    const auto &states = m_model.states();

    if (!stmName.isEmpty()) {
        cout << "Note: discarding a state-machine name\n";
    }

    for (const auto &[name, state] : states) {
        cout << "Name: " << hlText(name) << "\n";
        cout << "Parent: " << hlText(state.parent) << "\n";
        for (std::size_t i = 0; i < state.substates.size(); i++) {
            cout << "Substates (" << i << ") [" << state.substates[i].initial << "]:\n";
            for (const auto &substate : state.substates[i].states) {
                cout << "    " << hlText(substate) << "\n";
            }
        }
        if (!state.transitions.empty()) {
            cout << "Transitions:\n";
            for (const auto &[target, events] : state.transitions) {
                cout << "==> " << hlText(target) << "\n";
                for (const auto &event : events) {
                    cout << "    ** " << hlText(event) << "\n";
                }
            }
        }
        if (!state.deferrals.empty()) {
            cout << "Deferrals:\n";
            for (const auto &event : state.deferrals) {
                cout << "    " << hlText(event) << "\n";
            }
        }
        cout << "\n";
    }

    cout.flush();
}

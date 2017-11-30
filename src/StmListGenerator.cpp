#include "StmListGenerator.h"

#include <QIODevice>
#include <QTextStream>

void StmListGenerator::generate(QIODevice &output, const ScName &stmName)
{
    QTextStream cout(&output);
    const auto &rootState = m_model.states()[""];

    if ( !stmName.isEmpty() ) {
        cout << "Note: discarding a state-machine name\n";
    }

    if ( rootState.substates.empty() ) {
        cout << "No state-machines found\n";
    } else {
        cout << "Available state-machines:\n";
        for ( const auto &stm : rootState.substates[0].states ) {
            cout << "    " << stm << "\n";
        }
    }

    cout.flush();
}

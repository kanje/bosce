#pragma once

#include "AbstractGenerator.h"

class QTextStream;

class PlantUmlGenerator final
        : public AbstractGenerator
{
    using AbstractGenerator::AbstractGenerator;

public:
    void generate(QIODevice &output, const ScName &stmName) override;

private:
    void addWelcomeNote(QTextStream &cout, const ScName &stmName);
    void generate(QTextStream &cout, const ScName &name, int indentLevel);
    const ScName &alias(const ScName &name);

private:
    void beginState(QTextStream &cout, QByteArray &indent, const ScName &name, const ScName &nameAlias);
    void endState(QTextStream &cout, QByteArray &indent);
    void deferralLink(QTextStream &cout, QByteArray &indent, const ScName &stateAlias,
                      const ScName &eventName);
    void transitionLink(QTextStream &cout, QByteArray &indent, bool isTransitionHighlighted,
                        const ScName &stateAliasOrigin, const ScName &stateAliasTarget,
                        const ScNameSet &eventNames);

private:
    QMap<ScName, ScName> m_alias;
    int m_nextAlias = 1;
};

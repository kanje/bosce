/*
 * Boost StateChart Extractor
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE or copy at
 *                      http://www.boost.org/LICENSE_1_0.txt)
 */

#pragma once

#include "AbstractGenerator.h"

class PlantUmlGenerator final : public AbstractGenerator
{
    using AbstractGenerator::AbstractGenerator;

public:
    void generate(std::ostream &output, const ScName &stmName) override;

private:
    void addWelcomeNote(std::ostream &output, const ScName &stmName);
    void generate(std::ostream &output, const ScName &name, int indentLevel);
    const ScName &alias(const ScName &name);

private:
    void beginState(std::ostream &output, std::string &indent, const ScName &name,
                    const ScName &nameAlias);
    void endState(std::ostream &output, std::string &indent);
    void deferralLink(std::ostream &output, std::string &indent, const ScName &stateAlias,
                      const ScName &eventName);
    void transitionLink(std::ostream &output, std::string &indent, bool isTransitionHighlighted,
                        const ScName &stateAliasOrigin, const ScName &stateAliasTarget,
                        const ScNameSet &eventNames);

private:
    std::map<ScName, ScName> m_alias;
    int m_nextAlias = 1;
};

/*
 * Boost StateChart Extractor
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE or copy at
 *                      http://www.boost.org/LICENSE_1_0.txt)
 */

#pragma once

#include "ScModel.h"

#include <iostream>

class AbstractGenerator
{
public:
    AbstractGenerator(const ScModel &model, const ScNameSet &highlightSet = {})
        : m_model(model)
        , m_highlightSet(highlightSet)
    {
    }
    virtual ~AbstractGenerator() = default;

    virtual void generate(std::ostream &output, const ScName &stmName) = 0;

public:
    bool isHighlighted(const ScName &name) const
    {
        return (m_highlightSet.find(name) != m_highlightSet.cend());
    }

protected:
    const ScModel &m_model;
    const ScNameSet m_highlightSet;
};

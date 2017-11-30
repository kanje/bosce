#pragma once

#include "ScModel.h"

class QIODevice;

class AbstractGenerator
{
public:
    AbstractGenerator(const ScModel &model, const ScNameSet &highlightSet = {})
        : m_model(model)
        , m_highlightSet(highlightSet)
    {
    }

    virtual void generate(QIODevice &output, const ScName &stmName) = 0;

public:
    bool isHighlighted(const ScName& name) const {
        return ( m_highlightSet.find(name) != m_highlightSet.cend() );
    }

protected:
    const ScModel &m_model;
    const ScNameSet m_highlightSet;
};

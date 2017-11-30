#pragma once

#include "AbstractGenerator.h"

class StmListGenerator final
        : public AbstractGenerator
{
    using AbstractGenerator::AbstractGenerator;

public:
    void generate(QIODevice &output, const ScName &stmName) override;
};

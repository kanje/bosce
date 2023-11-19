/*
 * Boost StateChart Extractor
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE or copy at
 *                      http://www.boost.org/LICENSE_1_0.txt)
 */

#pragma once

#include "AbstractGenerator.h"

class TextGenerator final : public AbstractGenerator
{
    using AbstractGenerator::AbstractGenerator;

public:
    void generate(std::ostream &output, const ScName &stmName) override;

private:
    ScName hlText(const ScName &name) const;
};

/*
 * Boost StateChart Extractor
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE or copy at
 *                      http://www.boost.org/LICENSE_1_0.txt)
 */

#pragma once

#include <cstdint>
#include <iostream>

class ScParser;

class ObjDumpParser final
{
public:
    explicit ObjDumpParser(ScParser &scParser);

public:
    void parse(std::istream &input, bool doStripInput);

private:
    bool parseFunctionDecl(char *&data, std::size_t size);
    bool parseFunctionCall(char *&data, std::size_t size);

private:
    ScParser &m_scParser;
};

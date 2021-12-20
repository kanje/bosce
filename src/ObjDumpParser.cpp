/*
 * Boost StateChart Extractor
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE or copy at
 *                      http://www.boost.org/LICENSE_1_0.txt)
 */

#include "ObjDumpParser.h"
#include "ParserHelpers.h"
#include "ScParser.h"

#include <boost/algorithm/string/replace.hpp>

ObjDumpParser::ObjDumpParser(ScParser &scParser)
    : m_scParser(scParser)
{
}

void ObjDumpParser::parse(std::istream &input, bool doStripInput)
{
    std::string originalBuf;
    std::string buf;

    while (input) {
        std::getline(input, buf);

        if (doStripInput) {
            // Copy is needed because parsing modifies the original string for
            // performance reasons.
            originalBuf = buf;
        }

        char *data = buf.data();
        bool isHandled = false;
        if (*data == ' ') {
            isHandled = parseFunctionCall(data, buf.size());
        } else {
            isHandled = parseFunctionDecl(data, buf.size());
        }

        if (doStripInput && isHandled) {
            boost::replace_all(originalBuf, "mpl_::na, ", "");
            std::cout << originalBuf;
        }
    }

    if (!input.eof()) {
        throw std::runtime_error("Input error");
    }
}

// 0000000005c5c5d2 <foo::bar<T>(void*)>:
bool ObjDumpParser::parseFunctionDecl(char *&data, std::size_t size)
{
    char *const endOfData = data + size - 2;

    if (!(expectAddress(data) && expectString(data, " <")))
        return false;

    if (!eqString(endOfData, ">:"))
        return false;
    *endOfData = 0;

    return m_scParser.parseFunctionDecl(data);
}

inline bool expectAsmBytes(char *&data)
{
    for (int i = 0; i < 7; i++) {
        if (!(isxdigit(data[0]) && isxdigit(data[1]) && isspace(data[2]))) {
            if (i == 0)
                return false;
            break;
        }
        data += 3;
    }

    while (isspace(*data))
        ++data;
    return true;
}

// 5c5c5e5:       e8 34 21 00 00          callq  5c5e71e <void foo::bar<T>(void*)>
// 5c5c5e5:       e8 34 21 00 00          call  5c5e71e <void foo::bar<T>(void*)>
bool ObjDumpParser::parseFunctionCall(char *&data, std::size_t size)
{
    const char *const endOfData = data + size - 1;

    if (!(expectAddress(data) && expectString(data, ":\t") && expectAsmBytes(data)
          && (expectString(data, "callq  ") || expectString(data, "call  ")) && expectAddress(data)
          && expectString(data, " <")))
        return false;

    if (!eqString(endOfData, ">"))
        return false;

    return m_scParser.parseFunctionCall(data);
}

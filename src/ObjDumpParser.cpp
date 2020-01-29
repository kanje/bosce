#include "ObjDumpParser.h"
#include "ParserHelpers.h"
#include "ScParser.h"

#include <QIODevice>
#include <boost/algorithm/string/replace.hpp>
#include <iostream>

ObjDumpParser::ObjDumpParser(ScParser &scParser)
    : m_scParser(scParser)
{
}

void ObjDumpParser::parse(QIODevice &input, bool doStripInput)
{
    std::string originalBuf;

    while (!input.atEnd()) {
        qint64 rcnt = input.readLine(m_buf, m_bufsz);
        Q_ASSERT(rcnt != m_bufsz - 1);
        if (rcnt <= 0) {
            std::cerr << "Input error" << std::endl;
            return;
        }

        if (doStripInput) {
            // Copy is needed because parsing modifies the original string for
            // performance reasons.
            originalBuf = m_buf;
        }

        char *data = m_buf;
        bool isHandled = false;
        if (*data == ' ') {
            isHandled = parseFunctionCall(data, rcnt);
        } else {
            isHandled = parseFunctionDecl(data, rcnt);
        }

        if (doStripInput && isHandled) {
            boost::replace_all(originalBuf, "mpl_::na, ", "");
            std::cout << originalBuf;
        }
    }
}

// 0000000005c5c5d2 <foo::bar<T>(void*)>:
bool ObjDumpParser::parseFunctionDecl(char *&data, std::size_t size)
{
    char *const endOfData = data + size - 3;

    if (!(expectAddress(data) && expectString(data, " <")))
        return false;

    if (!eqString(endOfData, ">:\n"))
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
bool ObjDumpParser::parseFunctionCall(char *&data, std::size_t size)
{
    const char *const endOfData = data + size - 2;

    if (!(expectAddress(data) && expectString(data, ":\t") && expectAsmBytes(data)
          && expectString(data, "callq  ") && expectAddress(data) && expectString(data, " <")))
        return false;

    if (!eqString(endOfData, ">\n"))
        return false;

    return m_scParser.parseFunctionCall(data);
}

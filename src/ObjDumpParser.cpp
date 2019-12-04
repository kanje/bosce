#include "ObjDumpParser.h"
#include "ParserHelpers.h"
#include "ScParser.h"

#include <QIODevice>
#include <iostream>

ObjDumpParser::ObjDumpParser(ScParser &scParser)
    : m_scParser(scParser)
{
}

void ObjDumpParser::parse(QIODevice &input)
{
    while (!input.atEnd()) {
        qint64 rcnt = input.readLine(m_buf, m_bufsz);
        Q_ASSERT(rcnt != m_bufsz - 1);
        if (rcnt <= 0) {
            std::cerr << "Input error" << std::endl;
            return;
        }

        char *data = m_buf;
        if (*data == ' ') {
            parseFunctionCall(data, rcnt);
        } else {
            parseFunctionDecl(data, rcnt);
        }
    }
}

// 0000000005c5c5d2 <foo::bar<T>(void*)>:
void ObjDumpParser::parseFunctionDecl(char *&data, std::size_t size)
{
    char *const endOfData = data + size - 3;

    if (!(expectAddress(data) && expectString(data, " <")))
        return;

    if (!eqString(endOfData, ">:\n"))
        return;
    *endOfData = 0;

    m_scParser.parseFunctionDecl(data);
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
void ObjDumpParser::parseFunctionCall(char *&data, std::size_t size)
{
    const char *const endOfData = data + size - 2;

    if (!(expectAddress(data) && expectString(data, ":\t") && expectAsmBytes(data)
          && expectString(data, "callq  ") && expectAddress(data) && expectString(data, " <")))
        return;

    if (!eqString(endOfData, ">\n"))
        return;

    m_scParser.parseFunctionCall(data);
}

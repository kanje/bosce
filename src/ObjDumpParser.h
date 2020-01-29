#pragma once

#include <cstdint>

class ScParser;
class QIODevice;

class ObjDumpParser final
{
public:
    ObjDumpParser(ScParser &scParser);

public:
    void parse(QIODevice &input, bool doStripInput);

private:
    bool parseFunctionDecl(char *&data, std::size_t size);
    bool parseFunctionCall(char *&data, std::size_t size);

private:
    ScParser &m_scParser;
    static constexpr std::size_t m_bufsz = 1048510; /* 1 MiB */
    char m_buf[m_bufsz];
};

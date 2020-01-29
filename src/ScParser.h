#pragma once

#include "ScModel.h"

class ScParser final
{
public:
    ScParser(ScModel &model);

public:
    bool parseFunctionDecl(char *&data);
    bool parseFunctionCall(char *&data);

private:
    void parseStateMachine(char *&data);
    void parseSimpleState(char *&data);
    bool parseReactMethod(char *&data);
    void parseReactionList(char *mplList);

private:
    ScModel &m_model;
    ScName m_currentState;
    ScName m_currentEvent;
    bool m_hasCurrentState;
};

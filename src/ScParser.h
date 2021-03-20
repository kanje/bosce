/*
 * Boost StateChart Extractor
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE or copy at
 *                      http://www.boost.org/LICENSE_1_0.txt)
 */

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

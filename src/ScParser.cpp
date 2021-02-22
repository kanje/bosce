#include "ScParser.h"
#include "ParserHelpers.h"
#include "ScModel.h"

#include <iostream>

ScParser::ScParser(ScModel &model)
    : m_model(model)
    , m_hasCurrentState(false)
{
}

template <std::size_t N>
inline bool expectStartsWith(char *&data, const char (&prefix)[N])
{
    return expectString(data, prefix);
}

template <std::size_t N>
inline bool expectEndsWith(const char *start, char *end, const char (&suffix)[N])
{
    const std::size_t dataLen = end - start;
    const std::size_t suffixLen = N - 1;
    if (dataLen >= suffixLen) {
        if (eqString(end - suffixLen, suffix)) {
            end[-suffixLen] = '\0';
            return true;
        }
    }
    return false;
}

inline char *matchAngleArgument(char *&data, bool skipToEnd = false)
{
    char *start = data;
    int nrAngles = 1;

    for (;;) {
        switch (*data) {
        case ',':
            if (!skipToEnd && (nrAngles == 1)) {
                *(data++) = '\0';
                if (*data == ' ') {
                    ++data;
                }
                return start;
            }
            break;
        case '>':
            if (!--nrAngles) {
                if (data[-1] == ' ') {
                    // handle a double closing angle, e.g. "foo<bar<1> >"
                    data[-1] = '\0';
                    ++data;
                } else {
                    *(data++) = '\0';
                }
                return start;
            }
            break;
        case '<':
            ++nrAngles;
            break;
        case '\0':
            return nullptr;
        }
        ++data;
    }
}

bool ScParser::parseFunctionDecl(char *&data)
{
    m_hasCurrentState = false;

    // boost::statechart::detail::reaction_result
    //     boost::statechart::simple_state< *state-spec* >::
    //         local_react<boost::mpl::list<
    //                         boost::statechart::custom_reaction< *event-name* >,
    //                         boost::statechart::deferral< * event-name * >,
    //                         mpl_::na, ..., mpl_::na> >(
    //                            boost::statechart::event_base const&, void const*)>

    if (expectStartsWith(data, "boost::statechart::")) {
        expectStartsWith(data, "detail::reaction_result boost::statechart::");
        // boost::statechart::state_machine<fsm::StateMachine, fsm::NotStarted, ...
        if (expectStartsWith(data, "state_machine<")) {
            parseStateMachine(data);
            return true;
        } else {
            // boost::statechart::simple_state<fsm::NotStarted, fsm::StateMachine, ...
            if (expectStartsWith(data, "simple_state<")) {
                parseSimpleState(data);
                return true;
            }
        }
        return false;
    } else {
        return parseReactMethod(data);
    }
}

bool ScParser::parseFunctionCall(char *&data)
{
    if (!m_hasCurrentState) {
        return false;
    }

    // boost::statechart::detail::safe_reaction_result
    //     boost::statechart::simple_state<...>::transit<*state-name*>()

    expectStartsWith(data, "boost::statechart::detail::safe_reaction_result ");
    if (expectStartsWith(data, "boost::statechart::simple_state<")) {
        matchAngleArgument(data, true);
        if (expectStartsWith(data, "::transit<")) {
            QByteArray target = matchAngleArgument(data);
            m_model.addTransition(target, m_currentEvent);
        } else if (expectStartsWith(data, "::discard_event()")) {
            m_model.addTransition(m_currentState, m_currentEvent);
        } else if (expectStartsWith(data, "::defer_event()")) {
            m_model.addDeferral(m_currentEvent);
        }
        return true;
    }
    return false;
}

void ScParser::parseStateMachine(char *&data)
{
    ScName name = matchAngleArgument(data);
    ScName initialState = matchAngleArgument(data);

    m_model.addStateMachine(name, initialState);
}

static ScNameList parseInitialSubstateList(char *mplList)
{
    ScNameList list;

    if (expectStartsWith(mplList, "boost::mpl::list")) {
        expectAddress(mplList);
        if (!expectString(mplList, "<")) {
            return list;
        }

        forever
        {
            char *substate = matchAngleArgument(mplList);
            if (!substate) { // end of list
                break;
            }
            if (expectStartsWith(substate, "mpl_::na")) { // default values started
                break;
            }

            list.push_back(substate);
        }
    } else {
        list.push_back(mplList);
    }

    return list;
}

void ScParser::parseReactionList(char *mplList)
{
    /*
     * ::local_react<boost::mpl::list<boost::statechart::custom_reaction< *event-name* >,
     *                                boost::statechart::deferral< * event-name * >,
     *                                mpl_::na, ..., mpl_::na> >(boost::statechart::event_base
     * const&, void const*)>
     */

    if (expectStartsWith(mplList, "boost::mpl::list")) {
        expectAddress(mplList);
        if (!expectString(mplList, "<")) {
            return;
        }

        forever
        {
            char *reaction = matchAngleArgument(mplList);
            if (!reaction) {
                break;
            }
            if (expectStartsWith(reaction, "mpl_::na")) { // default values started
                break;
            }

            if (expectStartsWith(reaction, "boost::statechart::transition<")) {
                QByteArray event = matchAngleArgument(reaction);
                QByteArray target = matchAngleArgument(reaction);
                m_model.addTransition(target, event);
            } else if (expectStartsWith(reaction, "boost::statechart::deferral<")) {
                QByteArray event = matchAngleArgument(reaction);
                m_model.addDeferral(event);
            }
        }
    }
}

void ScParser::parseSimpleState(char *&data)
{
    /*
     * Basic:
     *
     * simple_state<state-name, parent-state,
     *              boost::mpl::list<initial-substate, mpl_::na, ...>,
     *              (boost::statechart::history_mode)0>
     *
     * With orthogonal region in a parent state:
     *
     * simple_state<state-name,
     *              boost::statechart::simple_state<parent-state, grandparent-state,
     *                                              boost::mpl::list<initial-substate, mpl_::na,
     * ...>, (boost::statechart::history_mode)0>::orthogonal<(unsigned char)0>,
     *              boost::mpl::list<initial-substate, mpl_::na, ...>,
     *              (boost::statechart::history_mode)0>
     */

    ScName name = matchAngleArgument(data);

    bool isOrthogonalState = expectStartsWith(data, "boost::statechart::simple_state<");
    ScName parent = matchAngleArgument(data);
    int orthRegion = 0;

    if (isOrthogonalState) {
        matchAngleArgument(data, true); // skip to ::orthogonal
        if (!expectStartsWith(data, "::orthogonal<(unsigned char)")) {
            return;
        }
        orthRegion = std::atoi(data);
        matchAngleArgument(data, true); // skip to ','
        matchAngleArgument(data);       // skip to a list of initial substates
    }

    m_model.addState(name, parent, orthRegion, parseInitialSubstateList(matchAngleArgument(data)));

    matchAngleArgument(data, true); // skip history_mode

    if (expectStartsWith(data, "::local_react<")) {
        parseReactionList(matchAngleArgument(data));
    }
}

inline char *matchName(char *&data, char terminus)
{
    char *start = data;

    for (int nrAngles = 0; *data; ++data) {
        if (isalnum(*data) || (*data == ':') || (*data == '_')) {
            continue;
        }
        if (*data == '<') {
            ++nrAngles;
            continue;
        }
        if ((*data == '>') && (nrAngles > 0)) {
            --nrAngles;
            continue;
        }
        if (nrAngles == 0) {
            break;
        }
    }

    if (*data != terminus) {
        return nullptr;
    }
    if (*data == '\0') {
        return start;
    }
    *(data++) = '\0';
    return start;
}

bool ScParser::parseReactMethod(char *&data)
{
    /*
     * *state*::react(*event* const&)
     */

    char *function = matchName(data, '(');
    if (!function) {
        return false;
    }
    if (!expectEndsWith(function, data - 1, "::react")) {
        return false;
    }

    m_currentState = function;
    m_currentEvent = matchName(data, ' ');
    if (m_currentEvent.isEmpty()) {
        return false;
    }

    m_hasCurrentState = true;
    m_model.setActiveState(m_currentState);
    return true;
}

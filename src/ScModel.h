/*
 * Boost StateChart Extractor
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE or copy at
 *                      http://www.boost.org/LICENSE_1_0.txt)
 */

#pragma once

#include <map>
#include <set>
#include <string>
#include <vector>

// Name of state-machine, state or event.
using ScName = std::string;

// Number of an orthogonal region.
using ScRegion = std::size_t;

// Set of names.
using ScNameSet = std::set<ScName>;
using ScNameList = std::vector<ScName>;

// Set of substates in one orthogonal region.
struct ScSubstateSet final
{
    // Initial state.
    ScName initial;

    // All substates.
    ScNameSet states;
};

// State or state-machine.
struct ScState final
{
    // Parent state name. If empty, it is a state-machine.
    ScName parent;

    // List of orthogonal substate sets.
    std::vector<ScSubstateSet> substates;

    // Set of transitions (target state -> what events lead there).
    std::map<ScName /* target */, ScNameSet /* set of events */> transitions;

    // Set of deferred events.
    ScNameSet deferrals;

    bool isDefined() const
    {
        return !parent.empty();
    }
};

// Map state name to state details.
using ScStateMap = std::map<ScName, ScState>;

class ScModel final
{
public:
    static const ScName RootScName;

public:
    void addStateMachine(const ScName &name, const ScName &initialState);
    void addState(const ScName &name, const ScName &parent, ScRegion orthRegion,
                  const ScNameList &initialSubstates);
    void addTransition(const ScName &target, const ScName &event);
    void addDeferral(const ScName &event);
    void setActiveState(const ScName &name);

public:
    const ScStateMap &states() const
    {
        return m_states;
    }

private:
    void addSubstate(const ScName &name, const ScName &substate, ScRegion orthRegion);

private:
    ScStateMap m_states;
    ScState *m_activeState = nullptr;
};

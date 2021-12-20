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

// Set of names.
using ScNameSet = std::set<ScName>;
using ScNameList = std::vector<ScName>;

// Orthogonal region.
struct ScRegion final
{
    // Initial state.
    ScName initial;

    // All substates.
    ScNameSet states;
};

// List of orthogonal regions.
using ScRegionList = std::vector<ScRegion>;

// Number of an orthogonal region.
using ScRegionNum = ScRegionList::size_type;

// Map of transitions (target state -> what events lead there).
using ScTransitionMap = std::map<ScName, ScNameSet /* set of events */>;

// State or state-machine.
struct ScState final
{
    ScName parent; // If empty, it is a state-machine.
    ScRegionList regions;
    ScTransitionMap transitions;
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
    void addState(const ScName &name, const ScName &parent, ScRegionNum regionNum,
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
    void addSubstate(const ScName &name, const ScName &substate, ScRegionNum regionNum);

private:
    ScStateMap m_states;
    ScState *m_activeState = nullptr;
};

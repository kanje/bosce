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

// History mode. See boost statechart's history_mode.
enum class ScHistoryMode
{
    None,
    Shallow,
    Deep,
    Full
};

// History pseudo-state details.
struct ScHistoryState final
{
    ScHistoryMode mode;

    // Default (initial) target state.
    ScName initial;
};

// Transition target.
struct ScTarget final
{
    // Target state name.
    ScName name;

    // Is target a history pseudo-state?
    ScHistoryMode historyMode;

    explicit ScTarget(ScName name, ScHistoryMode historyMode = ScHistoryMode::None) noexcept
        : name(std::move(name))
        , historyMode(historyMode)
    {
    }

    bool operator<(const ScTarget &other) const noexcept
    {
        return std::tie(name, historyMode) < std::tie(other.name, other.historyMode);
    }
};

// Map of transitions (target state -> what events lead there).
using ScTransitionMap = std::map<ScTarget, ScNameSet /* set of events */>;

// State or state-machine.
struct ScState final
{
    ScName parent;               // If empty, it is a state-machine.
    ScHistoryState historyState; // History pseudo-state.
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
                  const ScNameList &initialSubstates, ScHistoryMode historyMode);
    void addTransition(const ScName &target, const ScName &event,
                       ScHistoryMode historyMode = ScHistoryMode::None);
    void addDeferral(const ScName &event);
    void setActiveState(const ScName &name);

public:
    const ScStateMap &states() const
    {
        return m_states;
    }

private:
    void addSubstate(const ScName &name, const ScName &substate, ScRegionNum regionNum);
    void addHistory(const ScName &name, const ScName &initial, ScHistoryMode mode) noexcept;

private:
    ScStateMap m_states;
    ScState *m_activeState = nullptr;
};

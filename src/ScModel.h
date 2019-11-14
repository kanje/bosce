#pragma once

#include <QMap>
#include <QByteArray>
#include <QVector>

#include <set> // we need a predefined order

// Name of state-machine, state or event.
using ScName = QByteArray;

// Set of names.
using ScNameSet = std::set<ScName>;
using ScNameList = QVector<ScName>;

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
    QVector<ScSubstateSet> substates;

    // Set of transitions (target state -> what events lead there).
    QMap<ScName /* target */, ScNameSet /* set of events */> transitions;

    // Set of deferred events.
    ScNameSet deferrals;

    bool isDefined() const {
        return !parent.isEmpty();
    }
};

// Map state name to state details.
using ScStateMap = QMap<ScName, ScState>;

class ScModel final
{
public:
    static const ScName RootScName;

public:
    void addStateMachine(const ScName &name, const ScName &initialState);
    void addState(const ScName &name, const ScName &parent, int orthRegion, const ScNameList &initialSubstates);
    void addTransition(const ScName &target, const ScName &event);
    void addDeferral(const ScName &event);
    void setActiveState(const ScName &name);

public:
    const ScStateMap &states() const
    {
        return m_states;
    }

private:
    void addSubstate(const ScName &name, const ScName &substate, int orthRegion);

private:
    ScStateMap m_states;
    ScState *m_activeState = nullptr;
};

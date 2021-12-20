#include "ScModel.h"

const ScName ScModel::RootScName = "[root]";

void ScModel::addStateMachine(const ScName &name, const ScName &initialState)
{
    addState(name, RootScName, 0, {initialState});
}

void ScModel::addState(const ScName &name, const ScName &parent, ScRegionNum regionNum,
                       const ScNameList &initialSubstates)
{
    auto &state = m_states[name];
    m_activeState = &state;

    if (!state.isDefined()) {
        state.parent = parent;

        const auto nrOrthRegions = initialSubstates.size();
        state.regions.resize(nrOrthRegions);

        for (std::size_t i = 0; i < nrOrthRegions; i++) {
            const ScName &substate = initialSubstates[i];
            ScRegion &substateSet = state.regions[i];

            substateSet.initial = substate;
            substateSet.states.insert(substate);
        }

        addSubstate(parent, name, regionNum);
    }
}

void ScModel::addTransition(const ScName &target, const ScName &event)
{
    m_activeState->transitions[target].insert(event);
}

void ScModel::addDeferral(const ScName &event)
{
    m_activeState->deferrals.insert(event);
}

void ScModel::addSubstate(const ScName &name, const ScName &substate, ScRegionNum regionNum)
{
    auto &substates = m_states[name].regions;
    if (substates.size() <= regionNum) {
        substates.resize(regionNum + 1);
    }
    substates[regionNum].states.insert(substate);
}

void ScModel::setActiveState(const ScName &name)
{
    m_activeState = &m_states[name];
}

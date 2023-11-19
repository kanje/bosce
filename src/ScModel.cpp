/*
 * Boost StateChart Extractor
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE or copy at
 *                      http://www.boost.org/LICENSE_1_0.txt)
 */

#include "ScModel.h"

const ScName ScModel::RootScName = "[root]";

void ScModel::addStateMachine(const ScName &name, const ScName &initialState)
{
    addState(name, RootScName, 0, {initialState}, ScHistoryMode::None);
}

void ScModel::addState(const ScName &name, const ScName &parent, ScRegionNum regionNum,
                       const ScTargetList &initialSubstates, ScHistoryMode historyMode)
{
    auto &state = m_states[name];
    m_activeState = &state;

    if (!state.isDefined()) {
        state.parent = parent;
        state.historyState.mode = historyMode;

        const auto nrOrthRegions = initialSubstates.size();
        state.regions.resize(nrOrthRegions);

        for (std::size_t i = 0; i < nrOrthRegions; i++) {
            const ScName &substate = initialSubstates[i].name;
            ScRegion &substateSet = state.regions[i];

            substateSet.initial = substate;
            substateSet.states.insert(substate);
            if (initialSubstates[i].historyMode != ScHistoryMode::None) {
                state.historyState.initial = substate;
            }
        }

        addSubstate(parent, name, regionNum);
    }
}

void ScModel::addTransition(const ScName &target, const ScName &event, ScHistoryMode historyMode)
{
    if (historyMode == ScHistoryMode::None) {
        m_activeState->transitions[ScTarget(target)].insert(event);
    } else {
        auto &targetState = m_states[target];
        if (targetState.isDefined()) {
            auto &parent = targetState.parent;
            m_activeState->transitions[ScTarget(parent, historyMode)].insert(event);
            addHistory(parent, target, historyMode);
        }
    }
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

void ScModel::addHistory(const ScName &name, const ScName &initial, ScHistoryMode mode) noexcept
{
    auto &historyState = m_states[name].historyState;
    historyState.initial = initial;
    historyState.mode = mode;
}

void ScModel::setActiveState(const ScName &name)
{
    m_activeState = &m_states[name];
}

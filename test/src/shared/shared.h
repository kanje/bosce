/*
 * Boost StateChart Extractor
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE or copy at
 *                      http://www.boost.org/LICENSE_1_0.txt)
 */

#pragma once

#include <boost/statechart/custom_reaction.hpp>
#include <boost/statechart/simple_state.hpp>
#include <boost/statechart/state_machine.hpp>
#include <boost/statechart/transition.hpp>

namespace sc = boost::statechart;

struct EvToggle : sc::event<EvToggle>
{
};

struct Off;

struct OnOff : sc::state_machine<OnOff, Off>
{
};

struct Off : sc::simple_state<Off, OnOff>
{
    using reactions = sc::custom_reaction<EvToggle>;
    sc::result react(const EvToggle &ev);
};

struct On : sc::simple_state<On, OnOff>
{
    using reactions = sc::transition<EvToggle, On>;
};

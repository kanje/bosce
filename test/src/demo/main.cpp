/*
 * Boost StateChart Extractor
 *
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE or copy at
 *                      http://www.boost.org/LICENSE_1_0.txt)
 *
 * This is a simple statechart implementation based on official boost::statechart
 * tutorial https://www.boost.org/doc/libs/release/libs/statechart/doc/tutorial.html
 */

#include <boost/mpl/list.hpp>
#include <boost/statechart/custom_reaction.hpp>
#include <boost/statechart/deferral.hpp>
#include <boost/statechart/simple_state.hpp>
#include <boost/statechart/state_machine.hpp>
#include <boost/statechart/transition.hpp>

namespace sc = boost::statechart;

bool g_flag = true;

struct EvStartStop : sc::event<EvStartStop>
{
};
struct EvReset : sc::event<EvReset>
{
};
struct EvPlayMusic : sc::event<EvPlayMusic>
{
};
struct EvTerminate : sc::event<EvTerminate>
{
};

struct Active;
struct Stopped;

struct StopWatch : sc::state_machine<StopWatch, Active>
{
};

struct Active : sc::simple_state<Active, StopWatch, Stopped>
{
    using reactions = boost::mpl::list<sc::transition<EvReset, Active>,
                                       sc::custom_reaction<EvPlayMusic>, sc::deferral<EvTerminate>>;

    sc::result react(const EvPlayMusic &ev);
};

struct Running : sc::simple_state<Running, Active>
{
    using reactions = sc::transition<EvStartStop, Stopped>;
};

struct Stopped : sc::simple_state<Stopped, Active>
{
    using reactions = sc::transition<EvStartStop, Running>;
};

struct MusicBoxMode : sc::simple_state<MusicBoxMode, StopWatch>
{
    using reactions = sc::transition<EvReset, Active>;
};

sc::result Active::react(const EvPlayMusic &)
{
    if (g_flag) {
        return transit<MusicBoxMode>();
    }
    return transit<Active>();
}

int main()
{
    StopWatch myWatch;
    myWatch.initiate();
    return 0;
}

#include "Engine.h"
#include <cstdlib>
#include <iostream>
#include <tuple>
#include <vector>
void require(bool b, const char *msg)
{
    if (!b)
    {
        std::cerr << msg << '\n';
        std::exit(1);
    }
}
using Event = std::tuple<int, int, int, int>;
std::vector<Event> pattern(bool randomProbability)
{
    prism::Settings settings;
    settings[prism::seq_enabled] = 1;
    settings[prism::seq_swing] = .2f;
    settings[prism::seq_length] = 4;
    for (int i = 0; i < 4; ++i)
        settings[size_t(prism::step1_probability + i * 4)] = randomProbability ? .5f : 1;
    prism::Sequencer seq;
    seq.reset(123);
    seq.configure(settings);
    std::vector<Event> events;
    int index = 0;
    auto send = [&](int s, int n, int v) { events.emplace_back(index, s, n, v); };
    seq.midi(0x90, 60, 100, send);
    for (index = 0; index < 48000; ++index)
        seq.tick(settings, 48000, 120, send);
    return events;
}
int main()
{
    prism::Settings p;
    p[prism::arp_enabled] = 1;
    p[prism::arp_gate] = .5f;
    prism::Sequencer seq;
    seq.reset(99);
    seq.configure(p);
    std::vector<Event> events;
    int index = 0;
    auto send = [&](int s, int n, int v) { events.emplace_back(index, s, n, v); };
    for (int n : {60, 64, 67})
        seq.midi(0x90, n, 100, send);
    for (index = 0; index < 30001; ++index)
        seq.tick(p, 48000, 120, send);
    std::vector<Event> on;
    for (auto e : events)
        if (std::get<1>(e) == 0x90)
            on.push_back(e);
    require(on.size() == 3 && std::get<0>(on[0]) == 0 && std::get<0>(on[1]) == 12000 &&
                std::get<0>(on[2]) == 24000,
            "arp sample scheduling");
    require(std::get<2>(on[0]) == 60 && std::get<2>(on[1]) == 64 && std::get<2>(on[2]) == 67,
            "arp up order");
    require(std::get<0>(events[1]) == 6000 && std::get<1>(events[1]) == 0x80, "arp gate");
    seq.midi(0xb0, 123, 0, send);
    auto before = events.size();
    for (index = 30001; index < 48000; ++index)
        seq.tick(p, 48000, 120, send);
    require(events.size() == before, "arp panic flush");
    auto straight = pattern(false);
    std::vector<int> starts;
    for (auto e : straight)
        if (std::get<1>(e) == 0x90)
            starts.push_back(std::get<0>(e));
    require(starts.size() > 3 && starts[0] == 0 && starts[1] == 7200 && starts[2] == 12000 &&
                starts[3] == 19200,
            "pattern swing sample positions");
    require(pattern(true) == pattern(true), "probability seed");
    p[prism::arp_latch] = 1;
    seq.reset(1);
    seq.configure(p);
    seq.midi(0x90, 60, 100, send);
    seq.midi(0x80, 60, 0, send);
    events.clear();
    for (index = 0; index < 13000; ++index)
        seq.tick(p, 48000, 120, send);
    require(events.size() >= 3, "latch");
    seq.sync(true, false, .6, true);
    events.clear();
    for (index = 0; index < 13000; ++index)
        seq.tick(p, 48000, 120, send);
    for (auto e : events)
        require(std::get<1>(e) != 0x90, "transport stop");
    std::cout << "Arp order/gate/exact sample timing, panic, swing boundaries, fixed-seed "
                 "probability, latch and host stop passed.\n";
}

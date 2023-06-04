#pragma once

// #include "allocationcount.h"
#include "event.h"
#include "generator3.h"
#include "module.h"

#include "measure.h"
#include <dispatch.h>

#include <variant>

namespace voidPtrBenchmark {

struct Event {
    void* addr;
    enum ID {
        None,
        Event1,
        Event2,
        Event3,
        Event4,
        Event5,
    } id{};
};

auto voidPtr = [] (Generator3<>& gen) {

    std::vector<Event> queue;
    std::vector<Event> queue2;

    auto sink = [&] (auto ev) {
        // UB, needs heap allocation
        if      constexpr (std::same_as<decltype(ev), Event1>) { queue.push_back(Event{new Event1{ev}, Event::Event1}); }
        else if constexpr (std::same_as<decltype(ev), Event2>) { queue.push_back(Event{new Event2{ev}, Event::Event2}); }
        else if constexpr (std::same_as<decltype(ev), Event3>) { queue.push_back(Event{new Event3{ev}, Event::Event3}); }
        else if constexpr (std::same_as<decltype(ev), Event4>) { queue.push_back(Event{new Event4{ev}, Event::Event4}); }
        else if constexpr (std::same_as<decltype(ev), Event5>) { queue.push_back(Event{new Event5{ev}, Event::Event5}); }
    };
    auto genEvent = [&] { gen(sink); };
    using SinkType = decltype(genEvent);

    Dispatch broker{
        Module2<Event1, SinkType>{genEvent},
        Module2<Event2, SinkType>{genEvent},
        Module2<Event3, SinkType>{genEvent},
        Module2<Event4, SinkType>{genEvent},
        Module2<Event5, SinkType>{genEvent},
    };

    auto eventForward = [&] (Event& ev) -> void {

        if      (ev.id == Event::ID::Event1) { auto ptr = static_cast<Event1*>(ev.addr); broker(*ptr); delete ptr; }
        else if (ev.id == Event::ID::Event2) { auto ptr = static_cast<Event2*>(ev.addr); broker(*ptr); delete ptr; }
        else if (ev.id == Event::ID::Event3) { auto ptr = static_cast<Event3*>(ev.addr); broker(*ptr); delete ptr; }
        else if (ev.id == Event::ID::Event4) { auto ptr = static_cast<Event4*>(ev.addr); broker(*ptr); delete ptr; }
        else if (ev.id == Event::ID::Event5) { auto ptr = static_cast<Event5*>(ev.addr); broker(*ptr); delete ptr; }
	};

    auto work = [&] {
        gen(sink);
        while (not gen.empty()) {
            std::swap(queue, queue2);
            for (auto&& ev : queue2) {
                eventForward(ev);
            }
            queue2.clear();
        }
        return gen.curr;
    };

    measure("VoidPtr", work);
};

} // ns
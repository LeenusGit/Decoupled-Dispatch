#pragma once

// #include "allocationcount.h"
#include "event.h"
#include "generator3.h"
#include "module.h"

#include "measure.h"
#include <dispatch.h>

#include <variant>

using Event = std::variant<
    Event1,
    Event2,
    Event3,
    Event4,
    Event5
>;

auto variant = [] (Generator3<>& gen) {

    std::vector<Event> queue;
    std::vector<Event> queue2;

    auto sink = [&] (auto ev) { 
        if constexpr (std::same_as<decltype(ev), BigEvent>) {
            queue.push_back(std::make_shared<BigEvent>(ev));
        }
        else {
            queue.push_back(ev);
        }
    };
    auto genEvent = [&] { gen(sink); };
    using SinkType = decltype(genEvent);

    // std::vector<nanoseconds> deltas;
    // deltas.reserve(100);

    // auto pushDelta = [&] (auto delta) {
    //     deltas.push_back(delta);
    // };

    Dispatch broker{
        Module2<Event1, SinkType>{genEvent},
        Module2<Event2, SinkType>{genEvent},
        Module2<Event3, SinkType>{genEvent},
        Module2<Event4, SinkType>{genEvent},
        Module2<Event5, SinkType>{genEvent},
    };

    gen(sink);

    auto work = [&] {
        while (not gen.empty()) {
            std::swap(queue, queue2);
            for (auto&& ev : queue2) {
                std::visit(broker, ev);
            }
            queue2.clear();
        }
        return gen.curr;
    };

    measure("Variant", work);
};

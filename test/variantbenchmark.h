#pragma once

// #include "allocationcount.h"
#include "event.h"
#include "generator2.h"
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

auto variant = [] (Generator<>& eventGenerate) {

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
    auto genEvent = [&] { eventGenerate(sink); };
    using SinkType = decltype(genEvent);

    std::vector<nanoseconds> deltas;
    deltas.reserve(100);

    auto pushDelta = [&] (auto delta) {
        deltas.push_back(delta);
    };

    Dispatch broker{
        Module1<Event1, SinkType,  decltype(pushDelta)>{genEvent, pushDelta},
        Module2<Event2, SinkType>{genEvent},
        Module2<Event3, SinkType>{genEvent},
        Module2<Event4, SinkType>{genEvent},
        Module2<Event5, SinkType>{genEvent},
    };

    auto work = [&] {
        eventGenerate(sink);
        for (size_t i = 0; i < eventGenerate.newEvents.size(); i++) {
            std::swap(queue, queue2);
            for (auto&& ev : queue2) {
                std::visit(broker, ev);
            }
            queue2.clear();
        }
        return deltas;
    };

    measure("Variant", work);
};

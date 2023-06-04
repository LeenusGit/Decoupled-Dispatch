#pragma once

// #include "allocationcount.h"
#include "event.h"
#include "generator3.h"
#include "module.h"

#include <dispatch.h>
#include <polycontainer.h>

#include "measure.h"

#include <fmt/chrono.h>
#include <chrono>

using namespace std::chrono;

auto polyvec = [] (Generator3<>& gen) {

    PolyContainer<
        std::vector<Event1>,
        std::vector<Event2>,
        std::vector<Event3>,
        std::vector<Event4>,
        std::vector<Event5>
    > queue{};

    auto queue2 = queue;

    auto pushQueue = &queue;
    auto pullQueue = &queue2;

    auto sink = [&] (auto ev) { pushQueue->push_back(ev); };
    auto genEvent = [&] { gen(sink); };
    using SinkType = decltype(genEvent);

    // std::vector<nanoseconds> deltas;
    // deltas.reserve(100);

    // auto pushDelta = [&] (auto delta) {
    //     deltas.push_back(delta);
    // };

    Dispatch broker{
        // Module1<Event1, SinkType,  decltype(pushDelta)>{genEvent, pushDelta},
        Module2<Event1, SinkType>{genEvent},
        Module2<Event2, SinkType>{genEvent},
        Module2<Event3, SinkType>{genEvent},
        Module2<Event4, SinkType>{genEvent},
        Module2<Event5, SinkType>{genEvent},
    };


    auto dispatch = [&] (auto&& ev) { broker(ev); };
    gen(sink);

    auto work = [&] {
        while (not gen.empty()) {
            std::swap(pullQueue, pushQueue);
            pullQueue->visit(dispatch);
            pullQueue->clear();
        }
        return gen.curr;
    };
    measure("PolyVec", work);
};

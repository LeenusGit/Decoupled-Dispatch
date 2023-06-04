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

auto baseline = [] (Generator3<>& gen) {

    auto genEvent = [&] { };
    using SinkType = decltype(genEvent);

    Dispatch broker{
        Module2<Event1, SinkType>{genEvent},
        Module2<Event2, SinkType>{genEvent},
        Module2<Event3, SinkType>{genEvent},
        Module2<Event4, SinkType>{genEvent},
        Module2<Event5, SinkType>{genEvent},
    };

    auto work = [&] {
        while (not gen.empty()) {
            gen(broker);
        }
        return gen.curr;
    };
    measure("Baseline", work);
};

#pragma once

#include "event.h"
#include <cstddef>

#include <chrono>
using namespace std::chrono;

static size_t totalEventsReceived = 0;

template <typename EventType, typename Func, typename DeltaSink>
struct Module1 {
    Func genEvent;
    DeltaSink deltaSink;
    time_point<high_resolution_clock> prev = high_resolution_clock::now();

    void operator()(const EventType&) {
        const auto now = high_resolution_clock::now();
        const auto delta = now - prev;
        deltaSink(duration_cast<nanoseconds>(delta));
        prev = now;
        genEvent();
        totalEventsReceived++;
    }
};

template <typename EventType, typename Func>
struct Module2 {

    Func genEvent;

    void operator()(const EventType&) {
        // gen(sink);
        genEvent();
        totalEventsReceived++;
    }
};

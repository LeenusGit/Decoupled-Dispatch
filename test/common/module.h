#pragma once

// #include "event.h"
#include <cstddef>

#include <chrono>
using namespace std::chrono;


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
    }
};

template <typename EventType, typename Func>
struct Module2 {

    Func genEvent;

    Module2(Func func) : genEvent{func} { }

    void operator()(const EventType&) {

        genEvent();
    }
};

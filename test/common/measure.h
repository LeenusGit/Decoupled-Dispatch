#pragma once

#include "allocationcount.h"
#include "module.h"

#include <fmt/chrono.h>
#include <chrono>
using namespace std::chrono;

auto measure = [] (std::string name, auto func) {
    const auto begin = std::chrono::high_resolution_clock::now();
    std::vector<nanoseconds> deltas = func();
    const auto end = high_resolution_clock::now();
    const auto dur = end - begin;

    fmt::print("{}\n", name);
    fmt::print("Events: {}\n", totalEventsReceived);

    nanoseconds sum{};
    using milliseconds = duration<double, std::milli>;

    for (auto delta : deltas) {
        sum += delta;
        // fmt::print("Delta: {}\n", duration_cast<milliseconds>(delta));
    }
    const auto avg = duration_cast<milliseconds>(sum) / deltas.size();
    fmt::print("avg: {}\n", avg);

    using milliseconds = duration<double, std::milli>;
    fmt::print("Duration: {}\n", duration_cast<milliseconds>(dur));
    // fmt::print("Events per ms: {}\n", totalEventsReceived / duration_cast<milliseconds>(dur).count());
    // fmt::print("Heap: {} MiB\n", allocated / 1024.0 / 1024);
    if (totalEventsReceived != 0) {
        fmt::print("{} per event\n", dur / totalEventsReceived);
    }
};

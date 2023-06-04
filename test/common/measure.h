#pragma once

// #include "allocationcount.h"
// #include "module.h"

#include <fmt/chrono.h>
#include <chrono>
using namespace std::chrono;

auto measure = [] (std::string name, auto func, size_t eventCount = 0) {
    const auto begin = std::chrono::high_resolution_clock::now();
    eventCount = func();
    const auto end = high_resolution_clock::now();
    const auto dur = end - begin;

    fmt::print("{}\n", name);
    fmt::print("Events: {}\n", eventCount);

    using milliseconds = duration<double, std::milli>;

    using milliseconds = duration<double, std::milli>;
    fmt::print("Duration: {}\n", duration_cast<milliseconds>(dur));
    // fmt::print("Heap: {} MiB\n", allocated / 1024.0 / 1024);
    if (eventCount != 0) {
        fmt::print("{} per event\n", dur / eventCount);
    }
};

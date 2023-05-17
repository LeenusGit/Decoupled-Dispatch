#pragma once

#include "event.h"
#include "derivedevent.h"

#include <fmt/core.h>

#include <random>
#include <algorithm>
#include <array>
#include <vector>
#include <stack>

template<bool derived = false>
struct Generator {

    static constexpr size_t runs = 100;
    std::array<int, 5> hist{};
    // size_t total = 0;
    std::vector<int> indexes {1, 2, 3, 4};
    std::vector<int> newEvents;
    size_t curr = 0;

    Generator(size_t events) {

        std::mt19937 gen{42};
        for (size_t i = 0; i < runs; i++) {

            for (size_t j = 0; j < events; j++) {
                std::shuffle(indexes.begin(), indexes.end(), gen);
                newEvents.insert(newEvents.end(), indexes.begin(), indexes.end());
            }
            newEvents.push_back(0);
        }
        fmt::print("Allocated {} GiB for indexes\n", newEvents.capacity() * sizeof(int) / 1024.0 / 1024 / 1024);
    }

    ~Generator() {
        for (int idx = 0; int count : hist) {
            fmt::print("{}: {}\n", idx++, count);
        }
    }
    
    void operator()(auto sink) {

        if (curr == newEvents.size()) {
            return;
        }

        const auto idx = newEvents.at(curr);
        curr++;

        if constexpr (derived) {
            if (idx == 0) { sink(DerivedEvent1{}); }
            if (idx == 1) { sink(DerivedEvent2{}); }
            if (idx == 2) { sink(DerivedEvent3{}); }
            if (idx == 3) { sink(DerivedEvent4{}); }
            if (idx == 4) { sink(DerivedEvent5{}); }
        }
        else {
            if (idx == 0) { sink(Event1{}); }
            if (idx == 1) { sink(Event2{}); }
            if (idx == 2) { sink(Event3{}); }
            if (idx == 3) { sink(Event4{}); }
            if (idx == 4) { sink(Event5{}); }
        }
        hist[idx]++;
        // total++;

        // fmt::print("Gen: {}\n", idx);
    }

    void reset() {
        curr = 0;
    }
};

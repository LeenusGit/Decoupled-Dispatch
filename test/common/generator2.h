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

    static constexpr size_t runs = 10;
    std::array<int, 5> hist{};
    // size_t total = 0;
    std::vector<int> indexes {1, 2, 3, 4};
    std::vector<int> newEvents;
    std::vector<int> eventCounts {0, 0, 0, 3};
    std::vector<int> newEventCounts;
    size_t curr = 0;
    size_t currEventsCount = 0;
    size_t eventRequests = 0;

    Generator(size_t events) {

        std::mt19937 gen{42};

        // for (size_t i = 0; i < runs * events * indexes.size(); i++) {
        //     if      (i % 3 == 0) { newEventCounts.push_back(2); }
        //     else if (i % 4 == 0) { newEventCounts.push_back(3); }
        //     else {  newEventCounts.push_back(0); }
        //     fmt::print("val: {}\n", newEventCounts.at(i));
        // }

        for (size_t i = 0; i < runs; i++) {

            for (size_t j = 0; j < events; j++) {
                std::shuffle(indexes.begin(), indexes.end(), gen);
                newEvents.insert(newEvents.end(), indexes.begin(), indexes.end());

                newEventCounts.insert(newEventCounts.end(), eventCounts.begin(), eventCounts.end());
            }
            newEvents.push_back(0);
            newEventCounts.insert(newEventCounts.end(), eventCounts.begin(), eventCounts.end());
        }

        // std::shuffle(newEventCounts.begin(), newEventCounts.end(), gen);
        fmt::print("Generated {} indexes\n", newEvents.size());
        fmt::print("Allocated {} GiB for indexes\n", newEvents.capacity() * sizeof(int) / 1024.0 / 1024 / 1024);
    }

    ~Generator() {
        for (int idx = 0; int count : hist) {
            fmt::print("{}: {}\n", idx++, count);
        }
        fmt::print("Total requested {}\n", eventRequests);
        fmt::print("newEventCounts {}\n", newEventCounts.size());
        int sum = std::accumulate(newEventCounts.begin(), newEventCounts.end(), 0);
        fmt::print("sum {}\n", sum);
    }

    size_t newEventsCount() {
        return newEventCounts.at(currEventsCount++);
    }
    
    void operator()(auto sink) {

        eventRequests++;

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
        currEventsCount = 0;
    }
};

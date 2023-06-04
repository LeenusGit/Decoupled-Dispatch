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
struct Generator3 {
    
    std::array<int, 5> hist{};
    std::vector<int> indexes{};
    std::vector<int> newEvents{};
    size_t curr = 0;
    size_t curr2 = 0;

    Generator3(size_t eventCount, size_t listenerCount) {


        std::mt19937 gen{42};
        std::uniform_int_distribution<size_t> dis{0, listenerCount - 1};


        for (size_t i = 0; i < eventCount; i++) {
            indexes.push_back(dis(gen));

            // if (i % 20 == 0) {
            //     newEvents.push_back(20);
            // }
            // else {
            //     newEvents.push_back(0);
            // }
            newEvents.push_back(1);
        }
    }

    ~Generator3() {
        for (int idx = 0; int count : hist) {
            fmt::print("{}: {}\n", idx++, count);
        }
        // for (const auto c : newEvents) {
        //     fmt::print("newEvents {}\n", c);
        // }
    }

    void operator()(auto sink) {

        const auto newEventCount = newEvents.at(curr2++);

        for (int i = 0; i < newEventCount; i++) {

            const auto idx = indexes.at(curr);
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
        }
    }

    void reset() {
        curr = 0;
        curr2 = 0;
    }

    bool empty() const { return curr == indexes.size(); }
};

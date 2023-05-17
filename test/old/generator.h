#pragma once

#include "event.h"

#include <fmt/core.h>

#include <random>
#include <algorithm>
#include <array>
#include <vector>
#include <stack>

struct Generator {

    Generator(size_t events) {

        for (size_t i = 0; i < events * 2; i++) {
            auto tmp = dis(gen);
            int idx = tmp;
            idx = std::clamp(std::abs(idx), 0, 9);
            indexes.push_back(idx);
        }

        for (size_t i = 0; i < events; i++) {

            int growth = dis(gen);
            growth = std::clamp(std::abs(growth), 0, 9);
            newEvents.push(growth);
        }
    }

    ~Generator() {
        for (int idx = 0; int count : hist) {
            fmt::print("{}: {}\n", idx++, count);
        }
    }
    
    void operator()(auto sink) {

        const auto iterations = newEvents.top();
        newEvents.pop();
        for (int i = 0; i < iterations; i++) {

            const auto idx = indexes.at(curr);
            curr++;
            // auto tmp = dis(gen);
            // int idx = tmp;
            // idx = std::clamp(std::abs(idx), 0, 9);
            if (idx == 0) { sink(Tick{}); }
            if (idx == 1) { sink(DataReadyRead{}); }
            if (idx == 2) { sink(StateChanged{}); }
            if (idx == 3) { sink(ConfigChanged{}); }
            if (idx == 4) { sink(CallBackEvent{}); }
            if (idx == 5) { sink(Event1{}); }
            if (idx == 6) { sink(Event2{}); }
            if (idx == 7) { sink(Event3{}); }
            if (idx == 8) { sink(Error{}); }
            if (idx == 9) { sink(BigEvent{}); }
            hist[idx]++;
            total++;
        }
    }

    void idx() {
        const auto idx = indexes.at(curr);
        curr++;
        // auto tmp = dis(gen);
        // int idx = tmp;
        // idx = std::clamp(std::abs(idx), 0, 9);
        hist[idx]++;
        total++;
    }

    auto operator()() {
        int tmp = growthDis(gen);
        tmp = std::clamp(tmp, 0, 2);
        return tmp;
    }

    std::mt19937 gen{42};
    std::normal_distribution<> dis{0, 2.5};
    // std::normal_distribution<> dis{0, 0.1};
    std::normal_distribution<> growthDis{1, 1};
    std::array<int, 10> hist{};
    size_t total = 0;
    std::vector<int> indexes;
    std::stack<int> newEvents;
    size_t curr = 0;
};

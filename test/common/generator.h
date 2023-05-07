#pragma once

#include "event.h"

#include <random>
#include <algorithm>
#include <array>

struct Generator {
    void operator()(auto sink) {
        auto tmp = dis(gen);
        int idx = tmp;
        idx = std::clamp(std::abs(idx), 0, 9);
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
    }
    auto operator()() {
        int tmp = growthDis(gen);
        tmp = std::clamp(tmp, 0, 2);
        return tmp;
    }

    std::mt19937 gen{42};
    std::normal_distribution<> dis{0, 2.5};
    std::normal_distribution<> growthDis{1, 1};
    std::array<int, 10> hist{};
};

static Generator eventGenerate;
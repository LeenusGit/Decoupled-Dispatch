#pragma once

#include "allocationcount.h"
#include "event.h"
#include "generator2.h"
#include "module.h"

#include <dispatch.h>
#include <polycontainer.h>

#include "measure.h"

#include <fmt/chrono.h>
#include <chrono>

using namespace std::chrono;

#include <optional>
template <typename T, size_t cap>
struct StaticVector {
    size_t size = 0;

    StaticVector() { }

    std::array<T, cap> data{};
    constexpr void push_back(T item) {
        if (size < cap) {
            data[size] = item;
            size++;
        }
    }
    constexpr void clear() {
        for (size_t i = 0; i < size; i++) {
            std::destroy_at(data.begin() + i);
        }
        size = 0;
    }

    T* begin() { return data.begin(); }
    T* end() { return data.begin() + size; }
};

template <typename T>
struct Container {
    T item;
    void push_back(T) {
        fmt::print("{} pushed\n", typeid(T).name());
    }

    T* begin() { return &item; }
    T* end() { return &item + 1; }

    void clear() {
        fmt::print("Cleared\n");
    };
};

auto polyarray = [] (Generator<>& eventGenerate) {

    static constexpr auto defaultSize = 10;
    PolyContainer<
        StaticVector<Event1, defaultSize>,
        StaticVector<Event2, defaultSize>,
        StaticVector<Event3, defaultSize>,
        StaticVector<Event4, defaultSize>,
        StaticVector<Event5, defaultSize>
    > queue{};

    auto queue2 = queue;

    auto pushQueue = &queue;
    auto pullQueue = &queue2;

    auto sink = [&] (auto ev) { pushQueue->push_back(ev); };
    auto genEvent = [&] { eventGenerate(sink); };
    using SinkType = decltype(genEvent);

    std::vector<nanoseconds> deltas;
    deltas.reserve(100);

    auto pushDelta = [&] (auto delta) {
        deltas.push_back(delta);
    };

    Dispatch broker{
        Module1<Event1, SinkType,  decltype(pushDelta)>{genEvent, pushDelta},
        Module2<Event2, SinkType>{genEvent},
        Module2<Event3, SinkType>{genEvent},
        Module2<Event4, SinkType>{genEvent},
        Module2<Event5, SinkType>{genEvent},
    };

    auto dispatch = [&] (auto&& ev) { broker(ev); };

    auto work = [&] {
        eventGenerate(sink);
        for (size_t i = 0; i < eventGenerate.newEvents.size(); i++) {
            std::swap(pullQueue, pushQueue);
            pullQueue->visit(dispatch);
            pullQueue->clear();
        }
        return deltas;
    };
    measure("PolyArray", work);
};

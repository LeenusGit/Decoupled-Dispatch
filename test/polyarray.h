#pragma once

// #include "allocationcount.h"
#include "event.h"
#include "generator3.h"
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
        // fmt::print("size: {}\n", size);
        if (size < cap) {
            data[size] = item;
            size++;
        }
        else {
            fmt::print("size: {} when {} pushed\n", size, typeid(T).name());
            std::abort();
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

auto polyarray = [] (Generator3<>& gen) {

    static constexpr auto defaultSize = 15;
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

    auto genEvent = [&] {
        gen(sink);
    };

    using SinkType = decltype(genEvent);

    Dispatch broker{
        Module2<Event1, SinkType>{genEvent},
        Module2<Event2, SinkType>{genEvent},
        Module2<Event3, SinkType>{genEvent},
        Module2<Event4, SinkType>{genEvent},
        Module2<Event5, SinkType>{genEvent},
    };

    auto dispatch = [&] (auto&& ev) { broker(ev); };
    gen(sink);

    auto work = [&] {
        while (not gen.empty()) {
            std::swap(pullQueue, pushQueue);
            pullQueue->visit(dispatch);
            pullQueue->clear();
        }
        return gen.curr;
    };
    measure("PolyArray", work, gen.curr);
};

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

int main(int argc, char** args) {

    static constexpr size_t def = 10'000'000;
    const size_t limit = argc == 2 ? std::stoi(args[1]) : def;
    Generator eventGenerate{limit};

    static constexpr auto defaultSize = 10;
    PolyContainer<
        // StaticVector<DataReadyRead, defaultSize>,
        // StaticVector<Tick, defaultSize>,
        // StaticVector<ConfigChanged, defaultSize>,
        // StaticVector<StateChanged, defaultSize>,

        std::vector<Event1>,
        std::vector<Event2>,
        std::vector<Event3>,
        std::vector<Event4>,
        std::vector<Event5>

        // StaticVector<Event1, defaultSize>,
        // StaticVector<Event2, defaultSize>,
        // StaticVector<Event3, defaultSize>,
        // StaticVector<Event4, defaultSize>,
        // StaticVector<Event5, defaultSize>


        // StaticVector<CallBackEvent, defaultSize>,
        // StaticVector<Error, defaultSize>,
        // StaticVector<BigEvent, 5>
    > queue{};

    auto queue2 = queue;

    auto pushQueue = &queue;
    auto pullQueue = &queue2;

    auto sink = [&] (auto ev) { pushQueue->push_back(ev); };
    using SinkType = decltype(sink);

    std::vector<nanoseconds> deltas;
    deltas.reserve(100);

    auto pushDelta = [&] (auto delta) {
        // fmt::print("Delta: {}\n", delta);
        deltas.push_back(delta);
    };

    Dispatch broker{
        Module1{sink, pushDelta, eventGenerate},
        Module2<Event2, SinkType>{sink, eventGenerate},
        Module2<Event3, SinkType>{sink, eventGenerate},
        Module2<Event4, SinkType>{sink, eventGenerate},
        Module2<Event5, SinkType>{sink, eventGenerate},
        // Module2<Event4, SinkType>{sink, eventGenerate},
        // Module2<Event5, SinkType>{sink, eventGenerate},
        // Module<Error, SinkType>(sink),
        // Module<DataReadyRead, SinkType>(sink),
        // Module<ConfigChanged, SinkType>(sink),
        // Module<StateChanged, SinkType>(sink),
        // Module<Event1, SinkType>(sink),
        // Module<Event2, SinkType>(sink),
        // Module<Event3, SinkType>(sink),
        // Module<BigEvent, SinkType>(sink),
        // Module<CallBackEvent, SinkType>(sink),
        // Module<Tick, SinkType>(sink),
    };

    auto dispatch = [&] (auto&& ev) { broker(ev); };

    const auto a = high_resolution_clock::now();
    const auto b = high_resolution_clock::now();
    const auto dur = b - a;
    fmt::print("a - b: {}\n", (b - a));


    auto work = [&] {
        eventGenerate(sink);
        for (size_t i = 0; i < eventGenerate.newEvents.size(); i++) {
            // broker(Event1{});
            std::swap(pullQueue, pushQueue);
            pullQueue->visit(dispatch);
            pullQueue->clear();
        }
        return deltas;
    };
    measure("PolyArray", work);

    // using milliseconds = duration<double, std::milli>;
    // nanoseconds sum{};
    // for (auto delta : deltas) {
    //     sum += delta;
    //     fmt::print("Delta: {}\n", duration_cast<milliseconds>(delta));
    // }
    // const auto avg = sum.count() / deltas.size();
    // fmt::print("avg: {} ns\n", avg);
    // fmt::print("Total events: {}\n", totalEventsReceived);
}

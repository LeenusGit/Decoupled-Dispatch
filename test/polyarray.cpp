#include "allocationcount.h"
#include "event.h"
#include "generator.h"
#include "module.h"

// #include <polyvector.h>
#include <dispatch.h>
#include <polycontainer.h>

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

int main() {

    static constexpr auto defaultSize = 10;
    PolyContainer<
        StaticVector<DataReadyRead, defaultSize>,
        StaticVector<Tick, defaultSize>,
        StaticVector<ConfigChanged, defaultSize>,
        StaticVector<StateChanged, defaultSize>,
        StaticVector<Event1, defaultSize>,
        StaticVector<Event2, defaultSize>,
        StaticVector<Event3, defaultSize>,
        StaticVector<CallBackEvent, defaultSize>,
        StaticVector<Error, defaultSize>,
        StaticVector<BigEvent, 5>
    > queue{};

    auto queue2 = queue;

    auto pushQueue = &queue;
    auto pullQueue = &queue2;

    auto sink = [&] (auto ev) { pushQueue->push_back(ev); };
    using SinkType = decltype(sink);

    Dispatch broker{
        Module<Error, SinkType>(sink),
        Module<DataReadyRead, SinkType>(sink),
        Module<ConfigChanged, SinkType>(sink),
        Module<StateChanged, SinkType>(sink),
        Module<Event1, SinkType>(sink),
        Module<Event2, SinkType>(sink),
        Module<Event3, SinkType>(sink),
        Module<BigEvent, SinkType>(sink),
        Module<CallBackEvent, SinkType>(sink),
        Module<Tick, SinkType>(sink),
    };

    static constexpr size_t limit = 10'000'000;

    auto dispatch = [&] (auto&& ev) { broker(ev); };

    const auto begin = std::chrono::high_resolution_clock::now();

    for (size_t i = 0; i < limit; i++) {
        eventGenerate(sink);
        std::swap(pullQueue, pushQueue);
        pullQueue->visit(dispatch);
        pullQueue->clear();
    }

    const auto end = high_resolution_clock::now();
    const auto dur = end - begin;
    fmt::print("\nPolyarray\n");
    fmt::print("Count: {}\n", totalEventsReceived);
    for (int idx = 0; int count : eventGenerate.hist) {
        fmt::print("{}: {}\n", idx++, count);
    }
    using milliseconds = duration<double, std::milli>;
    fmt::print("Duration: {}\n", duration_cast<milliseconds>(dur));
    fmt::print("Events per ms: {}\n", totalEventsReceived / duration_cast<milliseconds>(dur).count());
    fmt::print("Heap: {} MiB\n", allocated / 1024.0 / 1024);
    fmt::print("{} per event\n", dur / totalEventsReceived);
}

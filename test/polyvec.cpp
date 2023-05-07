#include "allocationcount.h"
#include "event.h"
#include "generator.h"
#include "module.h"

#include <polyvector.h>
#include <dispatch.h>

#include <fmt/chrono.h>

#include <chrono>
using namespace std::chrono;

int main() {

    PolyVector<
        Error,
        DataReadyRead,
        ConfigChanged,
        StateChanged,
        Event1,
        Event2,
        Event3,
        BigEvent,
        CallBackEvent,
        Tick
    > vec;
    auto vec2 = vec;

    auto sink = [&] (auto ev) { vec.push_back(ev); };
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
        // eventGenerate(dispatch);
        eventGenerate(sink);
        std::swap(vec, vec2);
        vec2.visit(dispatch);
        vec2.clear();
    }

    const auto end = high_resolution_clock::now();
    const auto dur = (end - begin);
    fmt::print("\nPolyvec\n");
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

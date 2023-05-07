#include "allocationcount.h"
#include <polyvector.h>
#include <dispatch.h>

#include "event.h"
#include "generator.h"

#include "module.h"

#include <fmt/chrono.h>

#include <chrono>

using namespace std::chrono;

// template<typename EventType, typename F>
// struct Module {
//     F sink;
//     Module(F sink) : sink{sink} { }
//     void operator()(EventType) {
//         totalEventsReceived++;
//         const int newEvents = eventGenerate();
//         for (int i = 0; i < newEvents; i++) {
//             eventGenerate(sink);
//         }
//     }
// };

// static size_t allocated = 0;
// void* operator new(size_t size) {
//     void* p = malloc(size);
//     if (p != nullptr) {
//         // fmt::print("Allocated: {}\n", size);
//         allocated += size;
//     }
//     return p;
// }
// void operator delete(void * p) {
//     free(p);
// }
// void operator delete(void* p, size_t) {
//     free(p);
// }

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
        eventGenerate(dispatch);
        std::swap(vec, vec2);
        vec2.visit(dispatch);
        vec2.clear();
    }

    const auto end = high_resolution_clock::now();
    const auto dur = duration_cast<microseconds>(end - begin);
    fmt::print("\nPolyvec\n");
    fmt::print("Count: {}\n", totalEventsReceived);
    for (int idx = 0; int count : eventGenerate.hist) {
        fmt::print("{}: {}\n", idx++, count);
    }
    using milliseconds = duration<double, std::milli>;
    fmt::print("Duration: {}\n", duration_cast<milliseconds>(dur));
    fmt::print("Events per ms: {}\n", totalEventsReceived / duration_cast<milliseconds>(dur).count());
    fmt::print("Heap: {} MiB\n", allocated / 1024.0 / 1024);
}

#include "allocationcount.h"
#include "event.h"
#include "generator.h"
#include "module.h"

#include <polyvector.h>
#include <dispatch.h>

#include <fmt/chrono.h>

#include <chrono>
using namespace std::chrono;

#include <variant>

using Event = std::variant<
    Error,
    DataReadyRead,
    ConfigChanged,
    StateChanged,
    Event1,
    Event2,
    Event3,
    // BigEvent,
    std::shared_ptr<BigEvent>,
    CallBackEvent,
    Tick
>;

int main() {

    std::vector<Event> queue;
    std::vector<Event> queue2;

    auto sink = [&] (auto ev) { 
        if constexpr (std::same_as<decltype(ev), BigEvent>) {
            queue.push_back(std::make_shared<BigEvent>(ev));
        }
        else {
            queue.push_back(ev);
        }
    };
    using SinkType = decltype(sink);

    Dispatch broker{
        Module<Error, SinkType>(sink),
        Module<DataReadyRead, SinkType>(sink),
        Module<ConfigChanged, SinkType>(sink),
        Module<StateChanged, SinkType>(sink),
        Module<Event1, SinkType>(sink),
        Module<Event2, SinkType>(sink),
        Module<Event3, SinkType>(sink),
        Module<std::shared_ptr<BigEvent>, SinkType>(sink),
        Module<CallBackEvent, SinkType>(sink),
        Module<Tick, SinkType>(sink),
    };

    static constexpr size_t limit = 10'000'000;

    // auto dispatch = [&] (auto&& ev) { broker(ev); };

    const auto begin = std::chrono::high_resolution_clock::now();

    for (size_t i = 0; i < limit; i++) {
        eventGenerate(sink);
        std::swap(queue, queue2);
        for (auto&& ev : queue2) {
            std::visit(broker, ev);
        }
        queue2.clear();
    }

    const auto end = high_resolution_clock::now();
    const auto dur = duration_cast<microseconds>(end - begin);
    fmt::print("\nVariant\n");
    fmt::print("Count: {}\n", totalEventsReceived);
    for (int idx = 0; int count : eventGenerate.hist) {
        fmt::print("{}: {}\n", idx++, count);
    }
    using milliseconds = duration<double, std::milli>;
    fmt::print("Duration: {}\n", duration_cast<milliseconds>(dur));
    fmt::print("Events per ms: {}\n", totalEventsReceived / duration_cast<milliseconds>(dur).count());
    fmt::print("Heap: {} MiB\n", allocated / 1024.0 / 1024);
}

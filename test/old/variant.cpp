#include "allocationcount.h"
#include "event.h"
#include "generator.h"
#include "module.h"

#include "measure.h"

#include <polyvector.h>
#include <dispatch.h>

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

int main(int argc, char** args) {

    static constexpr size_t def = 10'000'000;
    const size_t limit = argc == 2 ? std::stoi(args[1]) : def;
    Generator eventGenerate{limit};

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

    // static constexpr size_t limit = 10'000'000;

    // auto dispatch = [&] (auto&& ev) { broker(ev); };

    // const auto begin = std::chrono::high_resolution_clock::now();

    auto work = [&] {
        for (size_t i = 0; i < limit; i++) {
            eventGenerate(sink);
            std::swap(queue, queue2);
            for (auto&& ev : queue2) {
                std::visit(broker, ev);
            }
            queue2.clear();
        }
    };

    measure("Variant", work);

    // const auto end = high_resolution_clock::now();
    // const auto dur = end - begin;
    // fmt::print("\nVariant\n");
    // fmt::print("Count: {}\n", totalEventsReceived);
    // for (int idx = 0; int count : eventGenerate.hist) {
    //     fmt::print("{}: {}\n", idx++, count);
    // }
    // using milliseconds = duration<double, std::milli>;
    // fmt::print("Duration: {}\n", duration_cast<milliseconds>(dur));
    // fmt::print("Events per ms: {}\n", totalEventsReceived / duration_cast<milliseconds>(dur).count());
    // fmt::print("Heap: {} MiB\n", allocated / 1024.0 / 1024);
    // fmt::print("{} per event\n", dur / totalEventsReceived);
}

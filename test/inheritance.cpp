
#include "allocationcount.h"
#include "dispatch.h"
#include "event.h"
#include "module.h"

#include <fmt/chrono.h>

#include <vector>
#include <memory>

#include <chrono>
using namespace std::chrono;

template <EventId Id, typename U>
auto downcastAndForward(Event* base, auto& func) {
	if (base->id == Id) {
		func(*static_cast<U*>(base));
		return true;
	}
	return false;
}

int main() {

    std::vector<std::unique_ptr<Event>> vec;
    std::vector<std::unique_ptr<Event>> vec2;

    auto sink = [&] (auto ev) { vec.push_back(std::make_unique<decltype(ev)>(ev)); };
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

    auto eventForward = [&] (Event* base) -> void {
        if      (downcastAndForward<EventId::Tick, Tick>(base, broker)) { return; }
        else if (downcastAndForward<EventId::DataReadyRead, DataReadyRead>(base, broker)) { return; }
        else if (downcastAndForward<EventId::StateChanged, StateChanged>(base, broker)) { return; }
        else if (downcastAndForward<EventId::ConfigChanged, ConfigChanged>(base, broker)) { return; }
        else if (downcastAndForward<EventId::CallBackEvent, CallBackEvent>(base, broker)) { return; }
        else if (downcastAndForward<EventId::Event1, Event1>(base, broker)) { return; }
        else if (downcastAndForward<EventId::Event2, Event2>(base, broker)) { return; }
        else if (downcastAndForward<EventId::Event3, Event3>(base, broker)) { return; }
        else if (downcastAndForward<EventId::Error, Error>(base, broker)) { return; }
        else if (downcastAndForward<EventId::BigEvent, BigEvent>(base, broker)) { return; }
	};

    static constexpr size_t limit = 10'000'000;

    const auto begin = std::chrono::high_resolution_clock::now();

    for (size_t i = 0; i < limit; i++) {
        eventGenerate(sink);
        std::swap(vec, vec2);
        for (auto& ptr : vec2) {
            eventForward(ptr.get());
        }
        vec2.clear();
    }

    const auto end = high_resolution_clock::now();
    const auto dur = (end - begin);
    fmt::print("\nInheritance\n");
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

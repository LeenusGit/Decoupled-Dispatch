
#include "allocationcount.h"
#include "dispatch.h"
#include "event.h"
#include "module.h"
#include "measure.h"

#include <vector>
#include <memory>


template <EventId Id, typename U>
auto downcastAndForward(Event* base, auto& func) {
	if (base->id == Id) {
		func(*static_cast<U*>(base));
		return true;
	}
	return false;
}

int main(int argc, char** args) {

    static constexpr size_t def = 10'000'000;
    const size_t limit = argc == 2 ? std::stoi(args[1]) : def;
    Generator eventGenerate{limit};

    std::vector<std::unique_ptr<Event>> vec;
    std::vector<std::unique_ptr<Event>> vec2;

    auto sink = [&] (auto ev) { vec.push_back(std::make_unique<decltype(ev)>(ev)); };
    using SinkType = decltype(sink);

    std::vector<nanoseconds> deltas;
    deltas.reserve(100);

    auto pushDelta = [&] (auto delta) {
        // fmt::print("Delta: {}\n", delta);
        deltas.push_back(delta);
    };

    Dispatch broker{
        // Module<Error, SinkType>(sink),
        // Module<DataReadyRead, SinkType>(sink),
        // Module<ConfigChanged, SinkType>(sink),
        // Module<StateChanged, SinkType>(sink),
        Module1(sink, pushDelta, eventGenerate),
        Module2<Event2, SinkType>(sink, eventGenerate),
        Module2<Event3, SinkType>(sink, eventGenerate),
        Module2<Event4, SinkType>(sink, eventGenerate),
        Module2<Event5, SinkType>(sink, eventGenerate),
        // Module<BigEvent, SinkType>(sink),
        // Module<CallBackEvent, SinkType>(sink),
        // Module<Tick, SinkType>(sink),
    };

    auto eventForward = [&] (Event* base) -> void {
        // if      (downcastAndForward<EventId::Tick, Tick>(base, broker)) { return; }
        // else if (downcastAndForward<EventId::DataReadyRead, DataReadyRead>(base, broker)) { return; }
        // else if (downcastAndForward<EventId::StateChanged, StateChanged>(base, broker)) { return; }
        // else if (downcastAndForward<EventId::ConfigChanged, ConfigChanged>(base, broker)) { return; }
        // else if (downcastAndForward<EventId::CallBackEvent, CallBackEvent>(base, broker)) { return; }
        if      (downcastAndForward<EventId::Event1, Event1>(base, broker)) { return; }
        else if (downcastAndForward<EventId::Event2, Event2>(base, broker)) { return; }
        else if (downcastAndForward<EventId::Event3, Event3>(base, broker)) { return; }
        else if (downcastAndForward<EventId::Event4, Event4>(base, broker)) { return; }
        else if (downcastAndForward<EventId::Event5, Event5>(base, broker)) { return; }
        // else if (downcastAndForward<EventId::Error, Error>(base, broker)) { return; }
        // else if (downcastAndForward<EventId::BigEvent, BigEvent>(base, broker)) { return; }
	};

    // static constexpr size_t limit = 10'000'000;

    // const auto begin = std::chrono::high_resolution_clock::now();

    auto work = [&] {
        eventGenerate(sink);
        for (size_t i = 0; i < eventGenerate.newEvents.size(); i++) {
            std::swap(vec, vec2);
            for (auto& ptr : vec2) {
                eventForward(ptr.get());
            }
            vec2.clear();
        }
        return deltas;
    };

    measure("Inheritance", work);

    // nanoseconds sum{};
    // using milliseconds = duration<double, std::milli>;

    // for (auto delta : deltas) {
    //     sum += delta;
    //     fmt::print("Delta: {}\n", duration_cast<milliseconds>(delta));
    // }
    // const auto avg = sum.count() / deltas.size();
    // fmt::print("avg: {} ns\n", avg);
    // fmt::print("Total events: {}\n", totalEventsReceived);

    // const auto end = high_resolution_clock::now();
    // const auto dur = (end - begin);
    // fmt::print("\nInheritance\n");
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

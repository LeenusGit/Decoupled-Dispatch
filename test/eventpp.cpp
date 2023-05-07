
#include "inherited/event.h"
#include "module.h"

#include <eventpp/eventqueue.h>
#include <fmt/core.h>
#include <fmt/chrono.h>

#include <chrono>

using namespace std::chrono;

using EventPointer = std::shared_ptr<Event>;

struct EventPolicy
{
	static EventId getEvent(const EventPointer& event) {
		return event->id;
	}
};

template <EventId Id, typename U>
auto downcastAndForward(EventPointer base, auto& func) {
	if (base->id == Id) {
		func(*static_cast<U*>(base.get()));
		return true;
	}
	return false;
}

static size_t allocated = 0;
void* operator new(size_t size) {
    void* p = malloc(size);
    if (p != nullptr) {
        allocated += size;
    }
    return p;
}

void operator delete(void * p) {
    free(p);
}

void operator delete(void* p, size_t) {
    free(p);
}

int main() {
    eventpp::EventQueue<EventId, void(const EventPointer&), EventPolicy> queue;

	auto pushEvent = [&](auto ev) {
		queue.enqueue(EventId::Any, std::make_shared<decltype(ev)>());
	};
    using SinkType = decltype(pushEvent);

	auto mTick = Module<Tick, SinkType>{pushEvent};
	auto mData = Module<DataReadyRead, SinkType>{pushEvent};
	auto mState = Module<StateChanged, SinkType>{pushEvent};
	auto mConf = Module<ConfigChanged, SinkType>{pushEvent};
	auto mCb = Module<CallBackEvent, SinkType>{pushEvent};
	auto mEvent1 = Module<Event1, SinkType>{pushEvent};
	auto mEvent2 = Module<Event2, SinkType>{pushEvent};
	auto mEvent3 = Module<Event3, SinkType>{pushEvent};
	auto mError = Module<Error, SinkType>{pushEvent};
	auto mBig = Module<BigEvent, SinkType>{pushEvent};

	auto eventForward = [&] (EventPointer base) -> void {
		if      (downcastAndForward<EventId::Tick, Tick>(base, mTick)) { return; }
		else if (downcastAndForward<EventId::DataReadyRead, DataReadyRead>(base, mData)) { return; }
		else if (downcastAndForward<EventId::StateChanged, StateChanged>(base, mState)) { return; }
		else if (downcastAndForward<EventId::ConfigChanged, ConfigChanged>(base, mConf)) { return; }
		else if (downcastAndForward<EventId::CallBackEvent, CallBackEvent>(base, mCb)) { return; }
		else if (downcastAndForward<EventId::Event1, Event1>(base, mEvent1)) { return; }
		else if (downcastAndForward<EventId::Event2, Event2>(base, mEvent2)) { return; }
		else if (downcastAndForward<EventId::Event3, Event3>(base, mEvent3)) { return; }
		else if (downcastAndForward<EventId::Error, Error>(base, mError)) { return; }
		else if (downcastAndForward<EventId::BigEvent, BigEvent>(base, mBig)) { return; }
	};

	queue.appendListener(EventId::Any, eventForward);

    // queue.dispatch(EventId::Any, std::make_shared<Tick>());
    // totalEventsReceived = 0;

    // static constexpr size_t limit = 1000;
    static constexpr size_t limit = 10'000'000;
    auto dispatch = [&] (auto ev) { 
        queue.dispatch(EventId::Any, std::make_shared<decltype(ev)>());
    };

    const auto begin = high_resolution_clock::now();
    for (size_t i = 0; i < limit; i++) {
        eventGenerate(dispatch);
        queue.process();
    }
    const auto end = high_resolution_clock::now();
    const auto dur = duration_cast<microseconds>(end - begin);

    fmt::print("Eventpp\n");
    fmt::print("Events: {}\n", totalEventsReceived);
    for (int idx = 0; int count : eventGenerate.hist) {
        fmt::print("{}: {}\n", idx++, count);
    }
    using milliseconds = duration<double, std::milli>;
    fmt::print("Duration: {}\n", duration_cast<milliseconds>(dur));
    fmt::print("Events per ms: {}\n", totalEventsReceived / duration_cast<milliseconds>(dur).count());
    fmt::print("Heap: {} MiB\n", allocated / 1024.0 / 1024);
}

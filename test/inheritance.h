
#include "allocationcount.h"
#include "dispatch.h"
#include "derivedevent.h"
#include "module.h"
#include "measure.h"

#include <vector>
#include <memory>

template <EventId Id, typename U>
auto downcastAndForward(EventBase* base, auto& func) {
	if (base->id == Id) {
		func(*static_cast<U*>(base));
		return true;
	}
	return false;
}

auto inheritance = [] (Generator<true>& eventGenerate) {

    std::vector<std::unique_ptr<EventBase>> vec;
    std::vector<std::unique_ptr<EventBase>> vec2;

    auto sink = [&] (auto ev) { vec.push_back(std::make_unique<decltype(ev)>(ev)); };
    auto genEvent = [&] {
        eventGenerate(sink);
    };
    using SinkType = decltype(genEvent);

    std::vector<nanoseconds> deltas;
    deltas.reserve(100);

    auto pushDelta = [&] (auto delta) {
        deltas.push_back(delta);
    };

    Dispatch broker{
        Module1<DerivedEvent1, SinkType, decltype(pushDelta)>{genEvent, pushDelta},
        Module2<DerivedEvent2, SinkType>(genEvent),
        Module2<DerivedEvent3, SinkType>(genEvent),
        Module2<DerivedEvent4, SinkType>(genEvent),
        Module2<DerivedEvent5, SinkType>(genEvent),
    };

    auto eventForward = [&] (EventBase* base) -> void {
        if      (downcastAndForward<EventId::Event1, DerivedEvent1>(base, broker)) { return; }
        else if (downcastAndForward<EventId::Event2, DerivedEvent2>(base, broker)) { return; }
        else if (downcastAndForward<EventId::Event3, DerivedEvent3>(base, broker)) { return; }
        else if (downcastAndForward<EventId::Event4, DerivedEvent4>(base, broker)) { return; }
        else if (downcastAndForward<EventId::Event5, DerivedEvent5>(base, broker)) { return; }
	};

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
};

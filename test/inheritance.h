#pragma once

// #include "allocationcount.h"
#include "generator3.h"
#include "dispatch.h"
#include "derivedevent.h"
#include "module.h"
#include "measure.h"

#include <vector>
#include <memory>

auto inheritance = [] (Generator3<true>& gen) {

    std::vector<std::unique_ptr<EventBase>> vec;
    std::vector<std::unique_ptr<EventBase>> vec2;

    auto sink = [&] (auto ev) { vec.push_back(std::make_unique<decltype(ev)>(ev)); };
    auto genEvent = [&] {
        gen(sink);
    };
    using SinkType = decltype(genEvent);

    Dispatch broker{
        Module2<DerivedEvent1, SinkType>(genEvent),
        Module2<DerivedEvent2, SinkType>(genEvent),
        Module2<DerivedEvent3, SinkType>(genEvent),
        Module2<DerivedEvent4, SinkType>(genEvent),
        Module2<DerivedEvent5, SinkType>(genEvent),
    };

    auto eventForward = [&] (EventBase* base) -> void {
        if      (base->id == EventId::Event1) { broker(*static_cast<DerivedEvent1*>(base)); }
        else if (base->id == EventId::Event2) { broker(*static_cast<DerivedEvent2*>(base)); }
        else if (base->id == EventId::Event3) { broker(*static_cast<DerivedEvent3*>(base)); }
        else if (base->id == EventId::Event4) { broker(*static_cast<DerivedEvent4*>(base)); }
        else if (base->id == EventId::Event5) { broker(*static_cast<DerivedEvent5*>(base)); }
	};

    gen(sink);

    auto work = [&] {
        while (not gen.empty()) {
            std::swap(vec, vec2);
            for (auto& ptr : vec2) {
                eventForward(ptr.get());
            }
            vec2.clear();
        }
        return gen.curr;
    };

    measure("Inheritance", work);
};

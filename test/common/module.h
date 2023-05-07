#pragma once

#include <cstddef>
#include "generator.h"

static size_t totalEventsReceived = 0;

template<typename EventType, typename F>
struct Module {
    F sink;
    Module(F sink) : sink{sink} { }
    void operator()(EventType) {
        totalEventsReceived++;
        const int newEvents = eventGenerate();
        for (int i = 0; i < newEvents; i++) {
            eventGenerate(sink);
        }
    }
};

#pragma once

#include "event2.h"
#include "event3.h"

#include <fmt/core.h>

#include <type_traits>

struct NotEvent {};

template<typename F>
struct Module2 {

    F sink;
    Module2(F sink) : sink{sink} { }
    
    constexpr void operator()(Event2) {
        if (std::is_constant_evaluated() == false) {
            fmt::print("M2 Create Event3\n");
            sink(Event3{});
        }
    }
};

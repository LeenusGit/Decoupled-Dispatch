#pragma once
#include "event1.h"
#include "event2.h"
#include "event3.h"

#include <fmt/core.h>

#include <type_traits>
#include <functional>

struct Module1 {

    std::function<void(Event2)> sink;
    
    constexpr auto operator()(Event1) {
        if (std::is_constant_evaluated() == false) {
            fmt::print("M1 Create Event2\n");
            sink(Event2{});
        }
    }
    
    constexpr auto operator()(Event3) {
        if (std::is_constant_evaluated() == false) {
            fmt::print("M1 received Event3\n");
            std::exit(0);
        }
    }
};

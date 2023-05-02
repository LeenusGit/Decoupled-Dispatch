#include "polyvector.h"
#include "dispatch.h"

#include "module1.h"
#include "module2.h"

#include <fmt/core.h>

#include <vector>
#include <thread>
#include <type_traits>

void* operator new(size_t size) {
    void* p = malloc(size);
    if (p != nullptr) {
        fmt::print("Allocated: {}\n", size);
    }
    return p;
}

int main() {

    PolyVector<Event1, Event2, Event3> vec;
    auto vec2 = vec;
    auto pushEv = [&] (auto&& ev) { vec.push_back(ev); };

    Dispatch dispatch{
        Module1(pushEv),
        Module2{pushEv},
    };

    dispatch(Event1{});

    while (true) {
        std::swap(vec, vec2);
        vec2.visit([&] (auto&& ev) { dispatch(ev); });
        vec2.clear();
    }
}

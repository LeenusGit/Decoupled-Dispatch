#include <polyvector.h>
#include <dispatch.h>

#include <chrono>

#include <fmt/chrono.h>

using namespace std::chrono;

struct Event1 {};

static constexpr double limit = 1'000'000'000;

template<typename F>
struct Module1 {
    F sink;
    int counter = 0;
    time_point<steady_clock> begin = steady_clock::now();

    void operator()(Event1) {
        counter++;
        if (counter == limit) {
            auto end = steady_clock::now();
            fmt::print("Duration: {} ms\n", duration_cast<milliseconds>(end - begin).count());
            fmt::print("{} per event\n", duration_cast<nanoseconds>(end - begin) / limit);
            // std::exit(0);
            counter = 0;
            begin = steady_clock::now();
        }
        sink(Event1{});
    }
};

void* operator new(size_t size) {
    void* p = malloc(size);
    if (p != nullptr) {
        fmt::print("Allocated: {}\n", size);
    }
    return p;
}

int main() {

    PolyVector<Event1> vec;
    auto pushEv = [&] (auto&& ev) { vec.push_back(ev); };

    Dispatch dispatch{
        Module1<decltype(pushEv)>(pushEv),
    };

    dispatch(Event1{});

    auto vec2 = vec;

    while (true) {
        std::swap(vec, vec2);
        vec2.visit([&] (auto&& ev) { dispatch(ev); });
        vec2.clear();
    }
}

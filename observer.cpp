
#include "eventbroker.h"

#include <fmt/core.h>
#include <variant>
#include <concepts>
#include <tuple>
#include <optional>
#include <vector>
#include <array>

struct Tick {
    int delta{1};
};
struct Event1 { };
struct Event2 { };
struct Event3 {
    // std::array<char, 64> data{};
};

using NoEvent = std::monostate;
using Event = std::variant<
    NoEvent,
    Event1,
    Event2,
    Event3
>;

static constexpr bool isEmpty(const Event& ev) { return std::holds_alternative<NoEvent>(ev); }

static constexpr auto s1 = sizeof(Event1);
static constexpr auto s2 = sizeof(Event);
static constexpr auto s3 = sizeof(std::optional<Event>);

struct ToString {
    constexpr auto operator()(Event1) { return "Event1"; }
    constexpr auto operator()(Event2) { return "Event2"; }
    constexpr auto operator()(Event3) { return "Event3"; }
    constexpr auto operator()(NoEvent) { return "Empty event"; }
};

struct Module1 {
    Event operator()(Event1) {
        fmt::print("Mod 1 Got Event1\n");
        if (goToNext) {
            fmt::print("Mod 1 created Event2\n");
            return Event2{};
        }
        return NoEvent{};
    }
    auto operator()(Tick t, auto func) {
        counter += t.delta;
        if (counter >= 5) {
            counter = 0;
            func(Event1{});
            func(Event2{});
        }
    }

    bool goToNext = false;
    int counter = 0;
};

struct Module2 {
    void operator()(Event2) {
        fmt::print("Mod 2 Got Event2\n");
    }
};

struct Module3 {
    
    Event3 operator()(Event1) {
        fmt::print("Mod 3 Got Event1\n");
        return Event3{};
    }

    void operator()(Event2) {
        fmt::print("Mod 3 Got Event2\n");
    }

    // void operator()(Event3) {
    //     fmt::print("Mod 3 Got Event3\n");
    // }

};

int main() {


    std::vector<Event> events {
        Event1{},
        Event2{},
        Event3{},
    };

    Module1 m1{};
    m1.goToNext = true;
    Module2 m2{};
    Module3 m3{};

    auto broker = make_broker<NoEvent, Event1, Event2, Tick>(m1, m2);
    broker(Tick{5});

    // const auto tmp = sizeof(decltype(evD)::AnyEvent);

    // for (auto&& oldEv : events) {
    //     const auto newEvents = evD(oldEv);
    //     for (auto&& ev : newEvents) {
    //         fmt::print("new event: {}\n", std::visit(ToString{}, ev));
    //     }
    // }

    return 0;
}

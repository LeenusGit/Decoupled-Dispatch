
#include <fmt/core.h>
#include <variant>
#include <concepts>
#include <tuple>
#include <optional>
#include <vector>
#include <array>

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
            return Event2{};
        }
        return NoEvent{};
    }
    bool goToNext = false;
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

/////


template <typename T, typename U>
concept EventListener = requires (T val) {
    val(U{});
};

template <typename T, typename U>
concept EventProducer = requires (T val) {
    { val(U{}) } -> std::convertible_to<Event>;
};

// template <typename T>
// concept Event1Listener = requires (T val) {
//     val(Event1{});
// };

// static_assert(EventListener<Module1, Event1>);

// template <typename T>
// concept ReturnsNewEventOnEvent1 = requires (T val) {
//     { val(Event1{}) } -> std::convertible_to<Event>;
// };

// template <typename T>
// concept Event2Listener = requires (T val) {
//     val(Event2{});
// };

// static std::tuple listerners {
//     Module1{},
//     Module2{},
//     Module3{},
// };

template<typename... Types>
struct EventDispatcher {

    std::tuple<Types...> listeners;
    std::vector<Event> newEvents;
    size_t ignoreCount = 0;

    // void operator()(Event1 ev, auto listener) {
    //     if constexpr (ReturnsNewEventOnEvent1<decltype(listener)>) {
    //         const auto res = listener(ev);
    //         newEvents.push_back(res);
    //     }
    //     else if constexpr (Event1Listener<decltype(listener)>) {
    //         listener(ev);
    //     }
    //     else {
    //         ignoreCount++;
    //     }
    // }

    // void operator()(Event2 ev, auto listener) {
    //     if constexpr (Event2Listener<decltype(listener)>) {
    //         listener(ev);
    //     } else {
    //         ignoreCount++;
    //     }
    // }

    void operator()([[maybe_unused]] auto ev, [[maybe_unused]] auto listener) {

        if constexpr (EventProducer<decltype(listener), decltype(ev)>) {
            const auto res = listener(ev);
            if (isEmpty(res) == false) {
                newEvents.push_back(res);
            }
        }

        else if constexpr (EventListener<decltype(listener), decltype(ev)>) {
            listener(ev);
        }
        else {
            ignoreCount++;
        }
        // fmt::print(
        //     "Unimplemented dispatch for event: {}\n",
        //     typeid(decltype(ev)).name()
        // );

        // fmt::print(
        //     "Event of type: {} ignored by {}\n",
        //     typeid(decltype(ev)).name(),
        //     typeid(decltype(listener)).name()
        // );
    }

    void notify(auto ev) {
        auto func = [&](auto&&... args) {
            (this->operator()(ev, args), ...);
        };
        std::apply(func, listeners);
    }

    auto operator()(Event ev) {
        newEvents.clear();
        ignoreCount = 0;

        std::visit([&](auto ev) { return notify(ev); }, ev);
        if (ignoreCount >= std::tuple_size<decltype(listeners)>()) {
            fmt::print("{} ignored by every listener\n", std::visit(ToString{}, ev));
        }
        return newEvents;
    }
};


int main() {

    EventDispatcher<
        Module1,
        Module2,
        Module3
    > evD{};

    std::vector<Event> events {
        Event1{},
        Event2{},
        Event3{},
    };

    for (auto&& oldEv : events) {
        const auto newEvents = evD(oldEv);
        for (auto&& ev : newEvents) {
            fmt::print("new: {}\n", std::visit(ToString{}, ev));
        }
    }
    return 0;
}

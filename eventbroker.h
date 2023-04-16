#pragma once

#include <concepts>
#include <tuple>
#include <vector>
#include <variant>

#include <fmt/core.h>

template <typename T, typename U>
concept EventListener = requires (T val) {
    val(U{});
};

template <typename T, typename U, typename V>
concept EventProducer = requires (T val) {
    { val(U{}) } -> std::convertible_to<V>;
};

template<typename... Types> struct type_sequence {};
template<typename... Types> struct EventBroker {};

template<typename... EventTypes, typename... ObserverTypes>
struct EventBroker<type_sequence<EventTypes...>, type_sequence<ObserverTypes...>> {

    using AnyEvent = std::variant<EventTypes...>;
    using AnyEventContainer = std::vector<AnyEvent>;
    using ObserverContainer = std::tuple<ObserverTypes&...>;

    EventBroker(ObserverTypes&... obs) : listeners{obs...} { }

    ObserverContainer listeners;
    AnyEventContainer newEvents;
    size_t ignoreCount = 0;

    void operator()([[maybe_unused]] auto ev, [[maybe_unused]] auto listener) {

        if constexpr (EventProducer<decltype(listener), decltype(ev), AnyEvent>) {
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
    }

    void notify(auto ev) {
        auto func = [&](auto&&... args) {
            (this->operator()(ev, args), ...);
        };
        std::apply(func, listeners);
    }

    auto operator()(AnyEvent ev) {
        newEvents.clear();
        ignoreCount = 0;

        std::visit([&](auto ev) { return notify(ev); }, ev);
        if (ignoreCount >= std::tuple_size<decltype(listeners)>()) {
            fmt::print("Event ignored by every listener\n");
        }
        return newEvents;
    }
};

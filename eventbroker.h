#pragma once

#include <concepts>
#include <tuple>
#include <vector>
#include <variant>
#include <span>

#include <fmt/core.h>

template <typename T, typename U>
concept EventListener = requires (T val) {
    val(U{});
};

template <typename T, typename U, typename V>
concept EventProducer = requires (T val) {
    { val(U{}) } -> std::convertible_to<V>;
};

template <typename T, typename U>
concept Callback = requires (T func) {
    { func(U{}) };
};

template <typename T, typename U, typename V>
concept EventProducer2 = requires (T functor) {
    { functor(U{}, V{}) };
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

    struct EventSink {
        void operator()(AnyEvent ev) {
            fmt::print("Sink got event: {}\n", ev.index());
        }
    };

    auto operator()(auto ev, auto& listener) {

        if constexpr (EventProducer2<decltype(listener), decltype(ev), EventSink>) {
            listener(ev, EventSink{});
        }

        else if constexpr (EventProducer<decltype(listener), decltype(ev), AnyEvent>) {
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

template <typename... EventTypes, typename... ObserversTypes>
auto make_broker(ObserversTypes&... observers) {
    using Events = type_sequence<EventTypes...>;
    using Observers = type_sequence<ObserversTypes...>;
        return EventBroker<Events, Observers>{observers...};
}

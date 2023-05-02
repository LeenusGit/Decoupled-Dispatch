#pragma once

#include <concepts>
#include <tuple>
#include <vector>
#include <variant>
#include <span>

#include <fmt/core.h>

template <typename T, typename... Ts>
concept same_as_any_of = (... or std::same_as<T, Ts>);

template <typename... ElementTypes>
struct PolyVector {
    std::tuple<std::vector<ElementTypes>...> vectors;

    void push_back(auto ev) {
        auto func = [&](auto&&... args) {
            (forward_to(ev, args), ...);
        };
        std::apply(func, vectors);
    }

    template<same_as_any_of<ElementTypes...> T, typename U>
    void forward_to(T ev, std::vector<U>&queue) {

        if constexpr (std::same_as<T, U>) {
            queue.push_back(ev);
        }
    }
};

template <typename T, typename U>
concept EventListener = requires (T val) {
    val(U{});
};

// template <typename T, typename U, typename V>
// concept EventProducer = requires (T val) {
//     { val(U{}) } -> std::convertible_to<V>;
// };

template <typename T, typename U>
concept Callback = requires (T func) {
    { func(U{}) };
};

template <typename T, typename U, typename V>
concept EventProducer = requires (T functor, U event, V sink) {
    // { sink(event)};
    { functor(event, sink) };
};

template<typename... Types> struct type_sequence {};
template<typename... Types> struct EventBroker {};

template<typename... ElementTypes, typename... FuncTypes>
struct EventBroker<type_sequence<ElementTypes...>, type_sequence<FuncTypes...>> {

    using AnyEvent = std::variant<ElementTypes...>;
    using AnyEventContainer = std::vector<AnyEvent>;
    using ObserverContainer = std::tuple<FuncTypes&...>;

    EventBroker(FuncTypes&... obs) : listeners{obs...} { }

    ObserverContainer listeners;
    // AnyEventContainer newEvents;
    // EventQueue<EventTypes...> queue{};
    int ignoreCount = 0;

    auto forward_to(auto ev, auto& listener, auto sink) {

        if constexpr (EventProducer<decltype(listener), decltype(ev), decltype(sink)>) {
            listener(ev, sink);
        }

        // else if constexpr (EventProducer<decltype(listener), decltype(ev), AnyEvent>) {
        //     const auto res = listener(ev);
        //     if (isEmpty(res) == false) {
        //         newEvents.push_back(res);
        //     }
        // }

        else if constexpr (EventListener<decltype(listener), decltype(ev)>) {
            listener(ev);
        }
        else {
            fmt::print(
                "{} ignored by {}\n",
                typeid(decltype(ev)).name(),
                typeid(decltype(listener)).name()
            );
            // ignoreCount++;
        }
    }

    void notify(auto ev, auto sink) {
        auto func = [&](auto&&... args) {
            (forward_to(ev, args, sink), ...);
        };
        std::apply(func, listeners);
    }

    auto visit(auto& queue) {
        for (auto ev : queue) {
            notify(ev, [](auto) {});
        }
        queue.clear();
    }

    void drain(auto evQueue) {
        auto func = [&](auto&&... args) {
            (visit(args), ...);
        };
        std::apply(func, evQueue.queues);
    }

    // auto operator()(AnyEvent ev, auto sink) {
    //     newEvents.clear();
    //     ignoreCount = 0;

    //     std::visit([&](auto ev) { return notify(ev); }, ev);
    //     if (ignoreCount >= std::tuple_size<decltype(listeners)>()) {
    //         fmt::print("Event ignored by every listener\n");
    //     }
    //     return newEvents;
    // }
};

template <typename... ElementTypes, typename... ObserversTypes>
auto make_broker(ObserversTypes&... observers) {
    using Events = type_sequence<ElementTypes...>;
    using Observers = type_sequence<ObserversTypes...>;
        return EventBroker<Events, Observers>{observers...};
}

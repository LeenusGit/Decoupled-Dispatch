#pragma once

#include <concepts>
#include <tuple>

template <typename T, typename U>
concept can_push_back = requires(T& container, const U& item) {
    container.push_back(item);
};

template <typename... Containers>
struct PolyContainer {
    std::tuple<Containers...> containers;

    template <typename T>
    constexpr auto push_back(T item) {

        auto iterate = [&] (auto& container) {
            if constexpr (can_push_back<decltype(container), T>) {
                container.push_back(item);
            }
        };

        auto func = [&](auto&... args) {
            (iterate(args), ...);
        };
        std::apply(func, containers);
    }

    constexpr auto visit(auto visitFunc) {

        auto iterate = [&] (auto& container) {
            for (auto&& element : container) {
                visitFunc(element);
            }
        };
        auto func = [&](auto&... args) {
            (iterate(args), ...);
        };
        std::apply(func, containers);
    }

    constexpr auto clear() {
        auto func = [&](auto&&... args) {
            (args.clear(), ...);
        };
        std::apply(func, containers);
    }
};

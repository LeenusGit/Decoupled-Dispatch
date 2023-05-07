#pragma once

#include <concepts>
#include <tuple>
#include <vector>

template <typename T, typename... Ts>
concept same_as_any_of = (... or std::same_as<T, Ts>);

template <typename... ElementTypes>
struct PolyVector {
    std::tuple<std::vector<ElementTypes>...> vectors;

    template<same_as_any_of<ElementTypes...> T>
    constexpr std::vector<T>& get_vector() {
        return std::get<std::vector<T>>(vectors);
    }

    template<same_as_any_of<ElementTypes...> T>
    constexpr auto push_back(const T& item) {
        return get_vector<T>().push_back(item);
    }

    template<same_as_any_of<ElementTypes...> T>
    constexpr auto push_back(T&& item) {
        return get_vector<T>().push_back(std::forward<T>(item));
    }

    constexpr auto clear() {
        auto func = [&](auto&&... args) {
            (args.clear(), ...);
        };
        std::apply(func, vectors);
    }

    constexpr auto visit(auto visitFunc) {

        auto iterate = [&] (const auto& vector) {
            for (auto&& element : vector) {
                visitFunc(element);
            }
        };
        auto func = [&](auto&&... args) {
            (iterate(args), ...);
        };
        std::apply(func, vectors);
    }

    // template<same_as_any_of<ElementTypes...> T>
    // constexpr auto push_back_func() {
    //     return [&] (T in) { push_back(in); };
    // }
};

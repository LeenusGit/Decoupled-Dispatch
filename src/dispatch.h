#pragma once

#include <concepts>
#include <tuple>
#include <utility>
#include <any>

template <typename T, typename U>
concept a_takes_b = requires (T& func, const U& obj) {
    { func(obj) };
};

template <typename T, typename U>
concept ptr_to_functor_that_takes = requires (T& func, const U& obj) {
    func->operator()(obj);
};

template <typename... Ts>
struct Dispatch {
    using Functors = std::tuple<Ts...>;
    Functors funcs;

    constexpr explicit Dispatch(Ts&&... funcs) : funcs{std::forward<Ts>(funcs)...} { }

    constexpr void operator()(const auto& arg) {
        auto func = [&](auto&... funcs) {
            (forward_to(funcs, arg), ...);
        };
        std::apply(func, funcs);
    }

private:
    template <typename Functor, typename ArgumentType> 
    constexpr void forward_to(Functor& func, const ArgumentType& arg) {

        if constexpr (a_takes_b<Functor, ArgumentType>) {
            func(arg); 
        }
        else if constexpr (ptr_to_functor_that_takes<Functor, ArgumentType>) {
            func->operator()(arg);
        }
    }
};

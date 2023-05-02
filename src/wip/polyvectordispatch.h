#pragma once

#include "polyvector.h"
#include "dispatch.h"

template<typename... Types> struct type_sequence {};
template<typename... Types> struct PolyVectorDispatch {};

template<typename... ElementTypes, typename... FuncTypes>
struct PolyVectorDispatch<type_sequence<ElementTypes...>, type_sequence<FuncTypes...>> {
    PolyVector<ElementTypes...> inQueue;
    PolyVector<ElementTypes...> outQueue;

    Dispatch<FuncTypes...> dispatch;

    constexpr explicit PolyVectorDispatch(FuncTypes&... funcs) : dispatch{funcs...} { }

    template<same_as_any_of<ElementTypes...> T>
    constexpr auto push_back_func() {
        return [&] (T&& in) { inQueue.push_back(std::forward<T>(in)); };
    }
};

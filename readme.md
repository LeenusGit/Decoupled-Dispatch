# Decoupled Event Dispatching using Static Polymorphism

    Typically, the observer pattern is implemented so that the subject being observed is part of the object for which state changes are being observed (and communicated to the observers). This type of implementation is considered tightly coupled, forcing both the observers and the subject to be aware of each other and have access to their internal parts, creating possible issues of scalability, speed, message recovery and maintenance 

https://en.wikipedia.org/wiki/Observer_pattern

The problem with the idiomatic observer patter is that is very intrusive. Every observer has to register themselves as an observer to some observable, and every observable has to store a list of every registered listener.

It is possible to eliminate these dependencies with static polymorphism, that is, we can (at compile time) take a list of event types and a list of observer types, then we forward each received event to every observer that has defined a function that takes the event.

An simple example:
```c++
#include <dispatch.h>

#include <fmt/core.h>

struct Event1 { }; // Depends on nothing

struct Module1 {    // Depends on Event1, (and fmt)
    
    auto operator()(Event1) {
        fmt::print("Received Event1\n");
    }
};

struct Event2 { }; // Depends on nothing

struct Module2 { // Depends on Event2, (and fmt)
    
    auto operator()(Event2) {
        fmt::print("Received Event2\n");
    }
};

int main() {
    Dispatch broker{
        Module1{},
        Module2{},
    };

    broker(Event1{});
    broker(Event2{});
}
```
Output:
```log
Received Event1
Received Event2
```

Pros:

- Event1 and Event2 do not know that they are events. They do not know that listeners exist.
- Module1 and Module2 do not know that they are listeners. They only know of the specific event that they are interested in.
- No inheritance

Cons:

<!-- - Every event type has to be known at compile time -->
- Every listener type has to be known at compile time
- It is not possible to dynamically register/unregister a listener. If Module1 is no longer interested in Event1 then it has to ignore it.
- Requires c++20 concepts

## How is Dispatch implemented?

The whole implementation looks like this:

```c++

#include <concepts>
#include <tuple>
#include <utility>
#include <any>

template <typename T, typename U>
concept a_takes_b = requires (T& func, const U& obj) {
    { func(obj) };
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
    }
};
```

Dispatch is constructed with a variadic number of types. Each argument is stored in a tuple matching the input types. When `operator()` is called with an argument, Dispatch will iterate over the tuple and forward the given argument to each of stored types' `operator()`, if they have implemented such a function. We are using C++20 concepts to check for existance of the implemented function operator.

The tuple is visited using the `std::apply` and a fold expression. This [should](https://stackoverflow.com/questions/46056268/order-of-evaluation-for-fold-expressions) guarantee that the order in which each listener is provided, is also the order in which each provided listener will receive an argument:

```c++
Dispatch dispatch{
    Module1{},  // Will receive Event1 before Module2 (assuming Module2 has implemented operator()(Event2))
    Module2{},
};

dispatch(Event1{});
```

## Producing Events

It likely that an event listener would also like to produce an event. However, we can not give the listener an instance to the broker. This would cause a cyclic dependency:

```c++
Dispatch broker{
    Module1{/* Pass in the Dispatch instance we are creating here? */},
};
```

We need a way to store events before they are forwarded. Can we do this whilst keeping them decoupled form each other?

Possible solutions:

### Using an Enum

```c++
enum class Event {
    One,
    Two,
    ...,
};
using EventQueue = std::vector<Event>;
```
No polymorphism is required, the dispatch implementation can be replaced by a for-loop. However, An event can not contain any data.

### Using a Variant

```c++
using EventQueue = std::vector<std::variant<Event1, Event2...>>;
```

Every will event will require size of the largest event type. Implementing a visitor would be trivial:

```c++

Dispatch broker{
    Module1{},
    Module2{},
};

using EventQueue = std::vector<std::variant<Event1, Event2>>;

EventQueue queue{};
queue.push_back(Event1{});
queue.push_back(Event2{});


for (auto&& event : queue) {
    broker(event);
}
```

### Using Inheritance

```c++
struct Event {
    virtual ~Event() = default;
};

std::vector<EventBase>
```

<!-- ### Using a Tuple of Vectors -->

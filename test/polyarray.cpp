#include "allocationcount.h"
#include "event.h"
#include "generator.h"
#include "module.h"

// #include <polyvector.h>
#include <dispatch.h>

#include <fmt/chrono.h>

#include <chrono>
using namespace std::chrono;

#include <optional>
template <typename T, size_t size>
struct StaticVector {
    std::array<std::optional<T>, size> data{};
    constexpr void push_back(T item) {
        for (auto& entry : data) {
            if (entry.has_value() == false) {
                entry = item;
                // fmt::print("Inserted {}\n", typeid(T).name());
                return;
            }
        }
    }
    constexpr void clear() {
        for (auto& entry : data) {
            entry.reset();
        }
    }
};

template <typename T, typename... Ts>
concept same_as_any_of = (... or std::same_as<T, Ts>);

static constexpr auto maxSize = 10;

template <typename... ElementTypes>
struct PolyArray {
    std::tuple<StaticVector<ElementTypes, maxSize>...> vectors;


    template<same_as_any_of<ElementTypes...> T>
    constexpr StaticVector<T, maxSize>& get_vector() {
        return std::get<StaticVector<T, maxSize>>(vectors);
    }

    template<same_as_any_of<ElementTypes...> T>
    constexpr auto push_back(T item) {
        // fmt::print("Push {}\n", typeid(T).name());
        return get_vector<T>().push_back(item);
    }

    constexpr auto visit(auto visitFunc) {

        auto iterate = [&] (const auto& vector) {
            for (auto&& element : vector.data) {

                // fmt::print("Push {}\n", typeid(decltype(element)).name());
                if (element.has_value()) {
                    visitFunc(element.value());
                }
                else {
                    break;
                }
            }
        };
        auto func = [&](auto&... args) {
            (iterate(args), ...);
        };
        std::apply(func, vectors);
    }

    constexpr auto clear() {
        auto func = [&](auto&&... args) {
            (args.clear(), ...);
        };
        std::apply(func, vectors);
    }
};

int main() {

    PolyArray<
        Error,
        DataReadyRead,
        ConfigChanged,
        StateChanged,
        Event1,
        Event2,
        Event3,
        CallBackEvent,
        Tick,
        BigEvent
    > queue{};
    auto queue2 = queue;

    auto& pushQueue = queue;
    auto& pullQueue = queue2;

    auto sink = [&] (auto ev) { pushQueue.push_back(ev); };
    using SinkType = decltype(sink);

    Dispatch broker{
        Module<Error, SinkType>(sink),
        Module<DataReadyRead, SinkType>(sink),
        Module<ConfigChanged, SinkType>(sink),
        Module<StateChanged, SinkType>(sink),
        Module<Event1, SinkType>(sink),
        Module<Event2, SinkType>(sink),
        Module<Event3, SinkType>(sink),
        Module<BigEvent, SinkType>(sink),
        Module<CallBackEvent, SinkType>(sink),
        Module<Tick, SinkType>(sink),
    };

    static constexpr size_t limit = 10'000'000;

    auto dispatch = [&] (auto&& ev) { broker(ev); };

    const auto begin = std::chrono::high_resolution_clock::now();

    for (size_t i = 0; i < limit; i++) {
        eventGenerate(sink);
        std::swap(pushQueue, pullQueue);
        pullQueue.visit(dispatch);
        pullQueue.clear();
    }

    const auto end = high_resolution_clock::now();
    const auto dur = duration_cast<microseconds>(end - begin);
    fmt::print("\nPolyarray\n");
    fmt::print("Count: {}\n", totalEventsReceived);
    for (int idx = 0; int count : eventGenerate.hist) {
        fmt::print("{}: {}\n", idx++, count);
    }
    using milliseconds = duration<double, std::milli>;
    fmt::print("Duration: {}\n", duration_cast<milliseconds>(dur));
    fmt::print("Events per ms: {}\n", totalEventsReceived / duration_cast<milliseconds>(dur).count());
    fmt::print("Heap: {} MiB\n", allocated / 1024.0 / 1024);
}

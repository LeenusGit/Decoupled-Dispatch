
#include <dispatch.h>
#include <fmt/core.h>
#include <vector>

struct EventBase {
    virtual ~EventBase() = default;
};

struct Event1 : public EventBase {};
struct Event2 : public EventBase {};

struct Module1 {
    
    auto operator()(Event1) {
        fmt::print("Received Event1\n");
    }
};

struct Module2 {
    
    auto operator()(Event2) {
        fmt::print("Received Event2\n");
    }
};

template <typename T>
auto downCast(EventBase* base, auto func) {
    const auto* res = dynamic_cast<T*>(base);
    if (res != nullptr)  {
        func(*res);
    }
}

int main() {
    std::vector<EventBase*> queue;

    auto ev1 = Event1{};
    auto ev2 = Event2{};
    queue.push_back(&ev1);
    queue.push_back(&ev2);

    Dispatch broker{
        Module1{},
        Module2{},
    };

    for (const auto ev: queue) {
        downCast<Event1>(ev, broker);
        downCast<Event2>(ev, broker);
    }
}

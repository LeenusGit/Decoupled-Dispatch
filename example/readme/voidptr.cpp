
#include <dispatch.h>

#include <fmt/core.h>
#include <vector>

struct EventWrapper {
    void* event;
    enum {
        None,
        Event1,
        Event2,
    } id{};
};

struct Event1 {};
struct Event2 {};

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

int main() {
    std::vector<EventWrapper> queue;

    auto ev1 = Event1{};
    auto ev2 = Event2{};
    queue.push_back(EventWrapper{&ev1, EventWrapper::Event1});
    queue.push_back(EventWrapper{&ev2, EventWrapper::Event2});

    Dispatch broker{
        Module1{},
        Module2{},
    };

    for (const auto& ev: queue) {
        switch (ev.id) {
        case EventWrapper::Event1: {
            broker(*static_cast<Event1*>(ev.event));
            break;
        }
        case EventWrapper::Event2: {
            broker(*static_cast<Event2*>(ev.event));
            break;
        }
        default:
            break;
        }
    }
}

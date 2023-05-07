
#include <dispatch.h>

#include <fmt/core.h>
#include <vector>
#include <variant>


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
    using EventQueue = std::vector<std::variant<Event1, Event2>>;
    EventQueue queue;
    queue.push_back(Event1{});
    queue.push_back(Event2{});

    Dispatch broker{
        Module1{},
        Module2{},
    };

    for (const auto& ev: queue) {
        std::visit(broker, ev);
    }
}

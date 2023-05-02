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

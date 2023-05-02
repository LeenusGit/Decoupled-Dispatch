#include <dispatch.h>
#include <polyvector.h>

#include <fmt/core.h>

#include <functional>

struct Question { };
using Answer = std::function<void()>;

template <typename F>
struct Module1 {
    
    F sink;
    int answer = 42;

    auto operator()(Question) {
        sink([&] {fmt::print("Module1 says answer is {}\n", answer);} );
        // Note. The lamda can not be called after Module1 is destroyed
    }
};

struct Module2 {
    
    auto operator()(Answer func) { func(); }
};

int main() {

    PolyVector<Question, Answer> vec;
    vec.push_back(Question{});

    Dispatch broker{
        Module1{ [&](auto func) { vec.push_back(Answer{func}); }},
        Module2{},
    };

    vec.visit([&] (auto&& ev) { broker(ev); });

    fmt::print("Answer size: {}\n", sizeof(Answer));
}

void* operator new(size_t size) {
    void* p = malloc(size);
    if (p != nullptr) {
        fmt::print("Allocated: {}\n", size);
    }
    return p;
}

#include <fmt/core.h>
#include <dispatch.h>
#include <variant>

using UnionType = std::variant<int, std::string>;

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;


struct Event1 { }; // Depends on nothing

struct Module1 {    // Depends on Event1, (and fmt)
    
    auto operator()(Event1) {
        fmt::print("Module1 Received Event1\n");
    }
};

struct Module2 { // Depends on Event2, (and fmt)
    
    auto operator()(Event1) {
        fmt::print("Module2 Received Event1\n");
    }
};

void test1() {
    overloaded broker{
        Module1{},
        Module2{},
    };
    // broker(Event1{}); // Will not compile
}

int main() {

    // test1();

    overloaded broker {
        [] (std::string str)    { fmt::print("Received \"{}\"\n", str); },
        [] (int val)            { fmt::print("Received {}\n", val); },
    };

    std::visit(broker, UnionType{42});
    std::visit(broker, UnionType{"42"});
}

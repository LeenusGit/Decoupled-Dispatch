#include <fmt/core.h>
#include <dispatch.h>
#include <variant>

using UnionType = std::variant<int, std::string>;

int main() {

    Dispatch broker{
        [] (std::string str)    { fmt::print("Received \"{}\"\n", str); },
        [] (std::string str)    { fmt::print("Received \"{}\" again \n", str); },
        [] (int val)            { fmt::print("Received {}\n", val); },
    };

    std::visit(broker, UnionType{42});
    std::visit(broker, UnionType{"42"});
}

#include "polyarray.h"
#include "polyvec.h"
#include "inheritance.h"
#include "variantbenchmark.h"

int main(int argc, char** args) {

    static constexpr size_t def = 10'000'000;
    const size_t limit = argc == 2 ? std::stoi(args[1]) : def;

    auto run = [limit] (auto func) {
        Generator eventGenerate{limit};
        func(eventGenerate);
        eventGenerate.reset();
        totalEventsReceived = 0;
    };

    auto runInheritance = [limit] (auto func) {
        Generator<true> derivedEventGen{limit};
        func(derivedEventGen);
        derivedEventGen.reset();
        totalEventsReceived = 0;
    };

    run(polyvec);
    fmt::print("\n");
    run(polyarray);
    fmt::print("\n");
    run(variant);
    fmt::print("\n");
    runInheritance(inheritance);
}

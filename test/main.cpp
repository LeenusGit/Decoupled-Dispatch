#include "polyarray.h"
#include "polyvec.h"
#include "inheritance.h"
#include "variantbenchmark.h"
#include "voidptr.h"
#include "baseline.h"

int main(int argc, char** args) {

    static constexpr size_t def = 1'000'000;
    const size_t limit = argc == 2 ? std::stoi(args[1]) : def;

    auto run = [limit] (auto func) {
        Generator3<> gen3{limit, 5};
        fmt::print("\n");
        func(gen3);
    };

    run(baseline);
    run(polyarray);
    run(polyvec);
    run(variant);
    run(voidPtrBenchmark::voidPtr);
    fmt::print("\n");
    Generator3<true> gen3{limit, 5};
    inheritance(gen3);
}

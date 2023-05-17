#include "allocationcount.h"
#include "generator2.h"
#include "module.h"

#include "measure.h"

int main(int argc, char** args) {

    static constexpr size_t def = 10'000'000;
    const size_t limit = argc == 2 ? std::stoi(args[1]) : def;

    Generator eventGenerate(limit * 4);

    static size_t count = 0;

    auto work = [&] {
        auto sink = [&] (auto) { count++; };
        for (size_t i = 0; i < limit; i++) {
            eventGenerate(sink);
            // const int newEvents = eventGenerate();
            // for (int i = 0; i < newEvents; i++) {
            //     eventGenerate(sink);
            // }
        }
        totalEventsReceived = eventGenerate.total;
    };
    measure("Baseline", work);
}

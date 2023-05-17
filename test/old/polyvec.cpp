#include "event.h"
#include "generator2.h"
#include "module.h"

#include "measure.h"

#include <polyvector.h>
#include <dispatch.h>

int main(int argc, char** args) {

    static constexpr size_t def = 10'000'000;
    const size_t limit = argc == 2 ? std::stoi(args[1]) : def;
    Generator eventGenerate{limit};

    PolyVector<
        Event1,
        Event2,
        Event3,
        Event4,
        Event5
    > vec;
    auto vec2 = vec;

    auto sink = [&] (auto ev) { vec.push_back(ev); };
    using SinkType = decltype(sink);

    std::vector<nanoseconds> deltas;
    deltas.reserve(Generator::runs);

    auto pushDelta = [&] (auto delta) {
        deltas.push_back(delta);
    };

    Dispatch broker{
        Module1(sink, pushDelta, eventGenerate),
        Module2<Event2, SinkType>(sink, eventGenerate),
        Module2<Event3, SinkType>(sink, eventGenerate),
        Module2<Event4, SinkType>(sink, eventGenerate),
        Module2<Event5, SinkType>(sink, eventGenerate),

    };

    auto dispatch = [&] (auto&& ev) { broker(ev); };

    auto work = [&] {
        eventGenerate(sink);
        for (size_t i = 0; i < eventGenerate.newEvents.size(); i++) {
            std::swap(vec, vec2);
            vec2.visit(dispatch);
            vec2.clear();
        }
        return deltas;
    };

    measure("PolyVec", work);
}

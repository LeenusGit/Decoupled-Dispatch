#! /bin/bash

meson compile -C builddir-benchmark && \
./builddir-benchmark/test/benchmark-polyvec && \
./builddir-benchmark/test/benchmark-variant && \
./builddir-benchmark/test/benchmark-inheritance && \
./builddir-benchmark/test/benchmark-eventpp
# ./builddir-benchmark/test/benchmark-polyarray

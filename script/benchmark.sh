#! /bin/bash

count=$1

meson compile -C builddir-benchmark && \
# ./builddir-benchmark/test/benchmark-baseline $count && \
./builddir-benchmark/test/benchmark-polyvec $count && \
# ./builddir-benchmark/test/benchmark-variant $count && \
./builddir-benchmark/test/benchmark-inheritance $count && \
# ./builddir-benchmark/test/benchmark-eventpp $count && \
./builddir-benchmark/test/benchmark-polyarray $count

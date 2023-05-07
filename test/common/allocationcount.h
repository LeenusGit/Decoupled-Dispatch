#pragma once

#include <cstddef>
#include <cstdlib>

static size_t allocated = 0;
void* operator new(size_t size) {
    void* p = ::malloc(size);
    if (p != nullptr) {
        // fmt::print("Allocated: {}\n", size);
        allocated += size;
    }
    return p;
}
void operator delete(void * p) noexcept {
    ::free(p);
}
void operator delete(void* p, size_t) noexcept {
    ::free(p);
}

module CopiumEngine.Memory;

import Copium.Core;

import <cstdlib>;
import <new>;


void* operator new(Copium::uint64 size)
{
    if (size == 0)
        ++size; // avoid std::malloc(0) which may return nullptr on success

    if (void* ptr = std::malloc(size))
        return ptr;

    throw std::bad_alloc{};
}

void operator delete(void* ptr) noexcept
{
    std::free(ptr);
}

#pragma once

#include <pe/Defines.hpp>

namespace pe::core::utils
{
    size_t prevIdxCircularBuffer(size_t head, size_t size, size_t capacity, size_t idx)
    {
        const size_t sizeWithGuard = size + 1;
        const size_t prevOffset = (idx + capacity - 1 - head) % capacity;
        const size_t sizeCorrectedPrevOffset = prevOffset % sizeWithGuard;
        return (head + sizeCorrectedPrevOffset) % capacity;
    }
    
    size_t nextIdxCircularBuffer(size_t head, size_t size, size_t capacity, size_t idx)
    {
        const size_t sizeWithGuard = size + 1;
        const size_t nextOffset = (idx + capacity + 1 - head) % capacity;
        const size_t sizeCorrectedNextOffset = nextOffset % sizeWithGuard;
        return (head + sizeCorrectedNextOffset) % capacity;
    }
}
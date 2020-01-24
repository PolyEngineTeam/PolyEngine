#pragma once

#include <pe/Defines.hpp>

namespace pe::core::utils
{
    /** Obtains previous physical index in a circular buffer.
     * 
     * @param head Buffer head
     * @param size Buffer actual size
     * @param capacity Buffer total capacity
     * @param idx Buffer current index
     * @return Previous index in circular buffer.
     */
    size_t CORE_DLLEXPORT prevIdxCircularBuffer(size_t head, size_t size, size_t capacity, size_t idx);
    
    /** Obtains next physical index in a circular buffer.
     * 
     * @param head Buffer head
     * @param size Buffer actual size
     * @param capacity Buffer total capacity
     * @param idx Buffer current index
     * @return Next index in circular buffer.
     */
    size_t CORE_DLLEXPORT nextIdxCircularBuffer(size_t head, size_t size, size_t capacity, size_t idx);
}
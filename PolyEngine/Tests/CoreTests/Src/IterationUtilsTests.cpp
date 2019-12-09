#include <pe/Defines.hpp>
#include <catch.hpp>

#include <pe/core/utils/IterationUtils.hpp>
using namespace pe::core::utils;

TEST_CASE("Test for distant head and tail", "[IterationUtils]")
{
	const size_t head = 6;
	const size_t size = 5;
	const size_t capacity = 10;
	const size_t tail = (head + size) % capacity;
    UNUSED(tail);
    CHECK(nextIdxCircularBuffer(head, size, capacity, 6) == 7);
	CHECK(nextIdxCircularBuffer(head, size, capacity, 7) == 8);
	CHECK(nextIdxCircularBuffer(head, size, capacity, 8) == 9);
	CHECK(nextIdxCircularBuffer(head, size, capacity, 9) == 0);
	CHECK(nextIdxCircularBuffer(head, size, capacity, 0) == 1);
	CHECK(nextIdxCircularBuffer(head, size, capacity, 1) == 6);
}

TEST_CASE("Test for two elements of distance between head and tail", "[IterationUtils]")
{
	const size_t head = 2;
	const size_t size = 5;
	const size_t capacity = 7;
	const size_t tail = (head + size) % capacity;	
    UNUSED(tail);
    CHECK(nextIdxCircularBuffer(head, size, capacity, 2) == 3);
	CHECK(nextIdxCircularBuffer(head, size, capacity, 3) == 4);
	CHECK(nextIdxCircularBuffer(head, size, capacity, 4) == 5);
	CHECK(nextIdxCircularBuffer(head, size, capacity, 5) == 6);
	CHECK(nextIdxCircularBuffer(head, size, capacity, 6) == 0);
	CHECK(nextIdxCircularBuffer(head, size, capacity, 0) == 2);
}

TEST_CASE("Test for one element of distance between head and tail", "[IterationUtils]")
{
	const size_t head = 2;
	const size_t size = 5;
	const size_t capacity = 6;
	const size_t tail = (head + size) % capacity;
    UNUSED(tail);
    CHECK(nextIdxCircularBuffer(head, size, capacity, 2) == 3);
	CHECK(nextIdxCircularBuffer(head, size, capacity, 3) == 4);
	CHECK(nextIdxCircularBuffer(head, size, capacity, 4) == 5);
	CHECK(nextIdxCircularBuffer(head, size, capacity, 5) == 0);
	CHECK(nextIdxCircularBuffer(head, size, capacity, 0) == 1);
	CHECK(nextIdxCircularBuffer(head, size, capacity, 1) == 2);
}

TEST_CASE("IterationUtils2", "[IterationUtils]")
{
	const size_t head = 0;
	const size_t size = 5;
	const size_t capacity = 7;
	const size_t tail = (head + size) % capacity;	
    UNUSED(tail);
    CHECK(nextIdxCircularBuffer(head, size, capacity, 0) == 1);
	CHECK(nextIdxCircularBuffer(head, size, capacity, 1) == 2);
	CHECK(nextIdxCircularBuffer(head, size, capacity, 2) == 3);
	CHECK(nextIdxCircularBuffer(head, size, capacity, 3) == 4);
	CHECK(nextIdxCircularBuffer(head, size, capacity, 4) == 5);
	CHECK(nextIdxCircularBuffer(head, size, capacity, 5) == 0);
}
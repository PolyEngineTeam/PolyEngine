#include <pe/Defines.hpp>
#include <catch.hpp>

#include <pe/core/utils/IterationUtils.hpp>
using namespace pe::core::utils;

size_t slowNextIdxCircularBuffer(size_t head, size_t tail, size_t size, size_t capacity, size_t idx)
{
	if(idx == capacity - 1)
	{
		if(tail > 0)
			return tail - 1;
		return tail;	
	}
	else 
	{
		if(idx == tail - 1)	
			return tail;
		if(idx == tail)
			return head;
		return ++idx;
	}
    return SIZE_MAX;
}

size_t slowPrevIdxCircularBuffer(size_t head, size_t tail, size_t size, size_t capacity, size_t idx)
{
	if(idx == 0)
	{
		return capacity - 1;
	}
	else
	{
		if(idx == head)
			return tail;
		return --idx;
	}
    return SIZE_MAX; 
}

TEST_CASE("Distant head and tail, unwrapped", "[IterationUtils]")
{
	const size_t head = 6;
	const size_t size = 5;
	const size_t capacity = 10;
	const size_t tail = (head + size) % capacity;
	CHECK(nextIdxCircularBuffer(head, size, capacity, 6) == slowNextIdxCircularBuffer(head, tail, size, capacity, 6));
	CHECK(nextIdxCircularBuffer(head, size, capacity, 7) == slowNextIdxCircularBuffer(head, tail, size, capacity, 7));
	CHECK(nextIdxCircularBuffer(head, size, capacity, 8) == slowNextIdxCircularBuffer(head, tail, size, capacity, 8));
	CHECK(nextIdxCircularBuffer(head, size, capacity, 9) == slowNextIdxCircularBuffer(head, tail, size, capacity, 9));
	CHECK(nextIdxCircularBuffer(head, size, capacity, 0) == slowNextIdxCircularBuffer(head, tail, size, capacity, 0));
	CHECK(nextIdxCircularBuffer(head, size, capacity, 1) == slowNextIdxCircularBuffer(head, tail, size, capacity, 1));

	CHECK(prevIdxCircularBuffer(head, size, capacity, 1) == slowPrevIdxCircularBuffer(head, tail, size, capacity, 1));
	CHECK(prevIdxCircularBuffer(head, size, capacity, 0) == slowPrevIdxCircularBuffer(head, tail, size, capacity, 0));
	CHECK(prevIdxCircularBuffer(head, size, capacity, 9) == slowPrevIdxCircularBuffer(head, tail, size, capacity, 9));
	CHECK(prevIdxCircularBuffer(head, size, capacity, 8) == slowPrevIdxCircularBuffer(head, tail, size, capacity, 8));
	CHECK(prevIdxCircularBuffer(head, size, capacity, 7) == slowPrevIdxCircularBuffer(head, tail, size, capacity, 7));
}

TEST_CASE("Two elements of distance between head and tail, wrapped", "[IterationUtils]")
{
	const size_t head = 2;
	const size_t size = 5;
	const size_t capacity = 7;
	const size_t tail = (head + size) % capacity;	
	CHECK(nextIdxCircularBuffer(head, size, capacity, 2) == slowNextIdxCircularBuffer(head, tail, size, capacity, 2));
	CHECK(nextIdxCircularBuffer(head, size, capacity, 3) == slowNextIdxCircularBuffer(head, tail, size, capacity, 3));
	CHECK(nextIdxCircularBuffer(head, size, capacity, 4) == slowNextIdxCircularBuffer(head, tail, size, capacity, 4));
	CHECK(nextIdxCircularBuffer(head, size, capacity, 5) == slowNextIdxCircularBuffer(head, tail, size, capacity, 5));
	CHECK(nextIdxCircularBuffer(head, size, capacity, 6) == slowNextIdxCircularBuffer(head, tail, size, capacity, 6));
	CHECK(nextIdxCircularBuffer(head, size, capacity, 0) == slowNextIdxCircularBuffer(head, tail, size, capacity, 0));
	
	CHECK(prevIdxCircularBuffer(head, size, capacity, 0) == slowPrevIdxCircularBuffer(head, tail, size, capacity, 0));
	CHECK(prevIdxCircularBuffer(head, size, capacity, 6) == slowPrevIdxCircularBuffer(head, tail, size, capacity, 6));
	CHECK(prevIdxCircularBuffer(head, size, capacity, 5) == slowPrevIdxCircularBuffer(head, tail, size, capacity, 5));
	CHECK(prevIdxCircularBuffer(head, size, capacity, 4) == slowPrevIdxCircularBuffer(head, tail, size, capacity, 4));
	CHECK(prevIdxCircularBuffer(head, size, capacity, 3) == slowPrevIdxCircularBuffer(head, tail, size, capacity, 3));
}

TEST_CASE("One element of distance between head and tail, wrapped", "[IterationUtils]")
{
	const size_t head = 2;
	const size_t size = 5;
	const size_t capacity = 6;
	const size_t tail = (head + size) % capacity;
	CHECK(nextIdxCircularBuffer(head, size, capacity, 2) == slowNextIdxCircularBuffer(head, tail, size, capacity, 2));
	CHECK(nextIdxCircularBuffer(head, size, capacity, 3) == slowNextIdxCircularBuffer(head, tail, size, capacity, 3));
	CHECK(nextIdxCircularBuffer(head, size, capacity, 4) == slowNextIdxCircularBuffer(head, tail, size, capacity, 4));
	CHECK(nextIdxCircularBuffer(head, size, capacity, 5) == slowNextIdxCircularBuffer(head, tail, size, capacity, 5));
	CHECK(nextIdxCircularBuffer(head, size, capacity, 0) == slowNextIdxCircularBuffer(head, tail, size, capacity, 0));
	CHECK(nextIdxCircularBuffer(head, size, capacity, 1) == slowNextIdxCircularBuffer(head, tail, size, capacity, 1));

	CHECK(prevIdxCircularBuffer(head, size, capacity, 1) == slowPrevIdxCircularBuffer(head, tail, size, capacity, 1));
	CHECK(prevIdxCircularBuffer(head, size, capacity, 6) == slowPrevIdxCircularBuffer(head, tail, size, capacity, 6));
	CHECK(prevIdxCircularBuffer(head, size, capacity, 5) == slowPrevIdxCircularBuffer(head, tail, size, capacity, 5));
	CHECK(prevIdxCircularBuffer(head, size, capacity, 4) == slowPrevIdxCircularBuffer(head, tail, size, capacity, 4));
	CHECK(prevIdxCircularBuffer(head, size, capacity, 3) == slowPrevIdxCircularBuffer(head, tail, size, capacity, 3));
}

TEST_CASE("Head at 0th index, unwrapped", "[IterationUtils]")
{
	const size_t head = 0;
	const size_t size = 5;
	const size_t capacity = 7;
	const size_t tail = (head + size) % capacity;	
	CHECK(nextIdxCircularBuffer(head, size, capacity, 0) == slowNextIdxCircularBuffer(head, tail, size, capacity, 0));
	CHECK(nextIdxCircularBuffer(head, size, capacity, 1) == slowNextIdxCircularBuffer(head, tail, size, capacity, 1));
	CHECK(nextIdxCircularBuffer(head, size, capacity, 2) == slowNextIdxCircularBuffer(head, tail, size, capacity, 2));
	CHECK(nextIdxCircularBuffer(head, size, capacity, 3) == slowNextIdxCircularBuffer(head, tail, size, capacity, 3));
	CHECK(nextIdxCircularBuffer(head, size, capacity, 4) == slowNextIdxCircularBuffer(head, tail, size, capacity, 4));
	CHECK(nextIdxCircularBuffer(head, size, capacity, 5) == slowNextIdxCircularBuffer(head, tail, size, capacity, 5));

	CHECK(prevIdxCircularBuffer(head, size, capacity, 5) == slowPrevIdxCircularBuffer(head, tail, size, capacity, 5));
	CHECK(prevIdxCircularBuffer(head, size, capacity, 4) == slowPrevIdxCircularBuffer(head, tail, size, capacity, 4));
	CHECK(prevIdxCircularBuffer(head, size, capacity, 3) == slowPrevIdxCircularBuffer(head, tail, size, capacity, 3));
	CHECK(prevIdxCircularBuffer(head, size, capacity, 2) == slowPrevIdxCircularBuffer(head, tail, size, capacity, 2));
	CHECK(prevIdxCircularBuffer(head, size, capacity, 1) == slowPrevIdxCircularBuffer(head, tail, size, capacity, 1));
}

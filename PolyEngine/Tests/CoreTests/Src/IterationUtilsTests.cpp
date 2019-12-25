#include <pe/Defines.hpp>
#include <catch.hpp>

#include <pe/core/utils/IterationUtils.hpp>
using namespace pe::core::utils;

size_t slowNextIdxCircularBuffer(size_t head, size_t tail, size_t size, size_t capacity, size_t idx)
{
	const size_t calcIdx = (idx + 1) % capacity;
	if (calcIdx == tail + 1) 
	{
		return head;
	}
	return calcIdx;
}

size_t slowPrevIdxCircularBuffer(size_t head, size_t tail, size_t size, size_t capacity, size_t idx)
{
	if (idx == 0)
	{
		if (head > tail)
			return capacity - 1;
		else
			return tail;
	} else if (idx == head)
		return tail;		
	return idx - 1;
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
	CHECK(prevIdxCircularBuffer(head, size, capacity, 6) == slowPrevIdxCircularBuffer(head, tail, size, capacity, 6));
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
	CHECK(prevIdxCircularBuffer(head, size, capacity, 2) == slowPrevIdxCircularBuffer(head, tail, size, capacity, 2));
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
	CHECK(prevIdxCircularBuffer(head, size, capacity, 2) == slowPrevIdxCircularBuffer(head, tail, size, capacity, 2));
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
	CHECK(prevIdxCircularBuffer(head, size, capacity, 0) == slowPrevIdxCircularBuffer(head, tail, size, capacity, 0));
}

TEST_CASE("Iterated container of capacity 10 with growing size and moving head", "[IterationUtils]")
{
	// outer loop increase head position, inner loop increase size 
	const size_t capacity = 10;
	for(size_t i = 1; i <= capacity; ++i)
	{
		for(size_t j = 1; j < i; ++j)
		{
			const size_t head = i - 1;
			const size_t size = j;
			const size_t tail = (head + size) % i;	
			size_t idx = head;
			// iterate through whole container
			for(size_t k = 0; k <= j; ++k)
			{
				INFO("Failed for values: i: " << i << " j: " << j << " k: " << k << " head: " << head << " tail: " << tail << " size: " << size << " idx: " << idx);
				CHECK(nextIdxCircularBuffer(head, size, i, idx) == slowNextIdxCircularBuffer(head, tail, size, i, idx));
				CHECK(prevIdxCircularBuffer(head, size, i, idx) == slowPrevIdxCircularBuffer(head, tail, size, i, idx));
				idx = nextIdxCircularBuffer(head, size, i, idx);
			}
		}
	}
}
#pragma once

#include <malloc.h>

template<size_t Aligment>
class AlignedAllocationPolicy
{
public:
	static void* operator new(size_t size)
	{
		return _aligned_malloc(size, Aligment);
	}

	static void operator delete(void* memory)
	{
		_aligned_free(memory);
	}
};
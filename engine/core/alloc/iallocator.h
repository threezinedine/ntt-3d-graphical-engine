#pragma once

#include "common.h"
#include "optional.h"
#

namespace ntt {

class IAllocator;

template <typename T>
struct Pointer
{
	T*			pPtr;
	IAllocator* pAllocator;
	u32			size;

	Pointer(T* ptr = nullptr, IAllocator* allocator = nullptr, u32 sz = 0)
		: pPtr(ptr)
		, pAllocator(allocator)
		, size(sz)
	{
	}

	Pointer(const Pointer& other)
		: pPtr(other.pPtr)
		, pAllocator(other.pAllocator)
		, size(other.size)
	{
	}

	Pointer(Pointer&& other) noexcept
		: pPtr(other.pPtr)
		, pAllocator(other.pAllocator)
		, size(other.size)
	{
		other.pPtr		 = nullptr;
		other.pAllocator = nullptr;
		other.size		 = 0;
	}

	Pointer& operator=(const Pointer& other)
	{
		if (this != &other)
		{
			pPtr	   = other.pPtr;
			pAllocator = other.pAllocator;
			size	   = other.size;
		}
		return *this;
	}

	Pointer& operator=(Pointer&& other) noexcept
	{
		if (this != &other)
		{
			pPtr			 = other.pPtr;
			pAllocator		 = other.pAllocator;
			size			 = other.size;
			other.pPtr		 = nullptr;
			other.pAllocator = nullptr;
			other.size		 = 0;
		}
		return *this;
	}

	~Pointer() {};

	inline T* Get() const
	{
		return pPtr;
	}

	T operator*() const
	{
		return pPtr;
	}

	T* operator->() const
	{
		return pPtr;
	}

	T* operator[](u32 index) const
	{
		return pPtr[index];
	}

	bool operator==(const Pointer& other) const
	{
		return pPtr == other.pPtr;
	}

	bool operator==(decltype(nullptr)) const
	{
		return pPtr == nullptr;
	}

	bool operator!=(const Pointer& other) const
	{
		return pPtr != other.pPtr;
	}

	bool operator!=(decltype(nullptr)) const
	{
		return pPtr != nullptr;
	}

	template <typename U>
	Pointer<U> Cast() const
	{
		return Pointer<U>{static_cast<U*>(pPtr), pAllocator, size};
	}

	Result Free();
};

class IAllocator
{
public:
	virtual ~IAllocator() = default;

	virtual Result		  Initialize()				= 0;
	virtual Pointer<void> Allocate(u32 size)		= 0;
	virtual Result		  Free(void* ptr, u32 size) = 0;
	virtual Result		  Shutdown()				= 0;
};

template <typename T>
Result Pointer<T>::Free()
{
	if (pPtr != nullptr)
	{
		NTT_ASSERT_MSG(pAllocator != nullptr, "Allocator is null. Cannot free memory.");
		NTT_ASSERT_RESULT_SUCCESS(pAllocator->Free((void*)pPtr, size));
		pPtr = nullptr;
	}

	return RESULT_SUCCESS;
}

} // namespace ntt
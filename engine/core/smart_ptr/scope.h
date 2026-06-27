#pragma once

#include <new>

#include "alloc/alloc.h"
#include "common.h"
#include "utils/utils.h"

namespace ntt {

template <typename T>
class Scope;

template <typename T>
Scope<T>&& move(Scope<T>& scope)
{
	return static_cast<Scope<T>&&>(scope);
}

template <typename T>
class Scope
{
public:
	Scope(T* ptr = nullptr, IAllocator* pAllocator = nullptr)
		: m_Ptr(ptr)
		, m_pAllocator(pAllocator)
	{
	}

	Scope(const Scope& other) = delete;

	Scope(Scope&& other) noexcept
		: m_Ptr(nullptr)
		, m_pAllocator(other.m_pAllocator)
	{
		if (ALLOCATOR_SAFE(m_pAllocator) != ALLOCATOR_SAFE(other.m_pAllocator) && other.m_Ptr != nullptr)
		{
			T* newPtr =
				new (ALLOCATOR_SAFE(m_pAllocator)->Allocate(sizeof(T))) T(*other.m_Ptr); // Move construct the object

			NTT_ASSERT(ALLOCATOR_SAFE(other.m_pAllocator)->Free(other.m_Ptr, sizeof(T)) == RESULT_SUCCESS);
			other.m_Ptr = nullptr; // Prevent double free
			m_Ptr		= newPtr;  // Update m_Ptr to point to the newly allocated memory
		}
		else
		{
			m_Ptr		= other.m_Ptr; // Move the pointer
			other.m_Ptr = nullptr;	   // Prevent double free
		}
	}

	Scope& operator=(const Scope& other) = delete;

	void operator=(Scope&& other)
	{
		if (this != &other)
		{
			m_Ptr->~T();

			if (ALLOCATOR_SAFE(m_pAllocator) != ALLOCATOR_SAFE(other.m_pAllocator) && other.m_Ptr != nullptr)
			{
				T* newPtr = new (ALLOCATOR_SAFE(m_pAllocator)->Allocate(sizeof(T)))
					T((T&&)*other.m_Ptr); // Move construct the object

				NTT_ASSERT(ALLOCATOR_SAFE(other.m_pAllocator)->Free(other.m_Ptr, sizeof(T)) == RESULT_SUCCESS);
				other.m_Ptr = nullptr; // Prevent double free
				m_Ptr		= newPtr;  // Update m_Ptr to point to the newly allocated memory
			}
			else
			{
				m_Ptr		= other.m_Ptr; // Move the pointer
				other.m_Ptr = nullptr;	   // Prevent double free
			}
		}
	}

	~Scope()
	{
		if (m_Ptr != nullptr)
		{
			m_Ptr->~T();
			NTT_ASSERT(ALLOCATOR_SAFE(m_pAllocator)->Free(m_Ptr, sizeof(T)) == RESULT_SUCCESS);
			m_Ptr = nullptr;
		}
	}

public:
	inline T* Get() const
	{
		return m_Ptr;
	}

private:
	T*			m_Ptr;
	IAllocator* m_pAllocator;
};

template <typename T, typename... Args>
Scope<T> MakeScope(IAllocator* pAllocator, Args&&... args)
{
	T* ptr = new (ALLOCATOR_SAFE(pAllocator)->Allocate(sizeof(T))) T(static_cast<Args&&>(args)...);
	return Scope<T>(ptr, pAllocator);
}

} // namespace ntt

#pragma once

#include "alloc/alloc.h"
#include "common.h"
#include "utils/utils.h"
#include <new>

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
	Scope(Pointer<T> ptr = Pointer<T>())
		: m_Ptr(ptr)
	{
	}

	Scope(const Scope& other) = delete;

	Scope(Scope&& other) noexcept
		: m_Ptr(other.m_Ptr)
	{
		other.m_Ptr = nullptr;
	}

	template <typename U>
	Scope(Scope<U>&& other) noexcept
	{
		m_Ptr		= other.m_Ptr.template Cast<T>();
		other.m_Ptr = nullptr;
	}

	Scope& operator=(const Scope& other) = delete;

	Scope& operator=(Scope&& other)
	{
		if (this != &other)
		{
			Reset();
			m_Ptr		= other.m_Ptr;
			other.m_Ptr = nullptr;
		}
		return *this;
	}

	~Scope()
	{
		Reset();
	}

	template <typename U>
	Scope& operator=(Scope<U>&& other)
	{
		Reset();

		m_Ptr		= other.m_Ptr.template Cast<T>();
		other.m_Ptr = nullptr;
		return *this;
	}

public:
	inline T* Get() const
	{
		return m_Ptr.Get();
	}

	inline Pointer<T> GetPointer() const
	{
		return m_Ptr;
	}

	T* operator*() const
	{
		return m_Ptr.Get();
	}

	T* operator->() const
	{
		return m_Ptr.Get();
	}

	bool operator==(const Scope& other) const
	{
		return m_Ptr == other.m_Ptr;
	}

	bool operator!=(const Scope& other) const
	{
		return m_Ptr != other.m_Ptr;
	}

	bool operator==(decltype(nullptr)) const
	{
		return m_Ptr == nullptr;
	}

	bool operator!=(decltype(nullptr)) const
	{
		return m_Ptr != nullptr;
	}

public:
	void Reset()
	{
		if (m_Ptr != nullptr)
		{
			m_Ptr.Get()->~T();
			NTT_ASSERT(m_Ptr.Free() == RESULT_SUCCESS);
			m_Ptr = nullptr;
		}
	}

	Result Transfer(IAllocator* pNewAllocator)
	{
		if (m_Ptr == nullptr)
		{
			return RESULT_NULL_POINTER;
		}

		Pointer<T> pNewPtr = ALLOCATOR_SAFE(pNewAllocator)->Allocate(sizeof(T)).Cast<T>();
		MemCopy(pNewPtr.Get(), m_Ptr.Get(), (u32)sizeof(T));
		NTT_ASSERT(m_Ptr.Free() == RESULT_SUCCESS);
		m_Ptr = pNewPtr;

		return RESULT_SUCCESS;
	}

	template <typename U>
	friend class Scope;

private:
	Pointer<T> m_Ptr;
};

template <typename T, typename... Args>
Scope<T> MakeScope(IAllocator* pAllocator, Args&&... args)
{
	T* ptrRaw = new (ALLOCATOR_SAFE(pAllocator)->Allocate(sizeof(T)).pPtr) T(static_cast<Args&&>(args)...);
	return Scope<T>(Pointer<T>(ptrRaw, ALLOCATOR_SAFE(pAllocator), sizeof(T)));
}

} // namespace ntt

void* operator new(size_t size);
void  operator delete(void* ptr);
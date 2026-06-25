#pragma once

#include "alloc/alloc.h"
#include "common.h"

#define NTT_ARRAY_DEFAULT_CAPACITY 4

namespace ntt {

template <typename T>
class Array
{
public:
	typedef bool (*Predicate)(const T&);

public:
	Array(u32 capacity = NTT_ARRAY_DEFAULT_CAPACITY, IAllocator* pAllocator = nullptr)
		: m_pData(nullptr)
		, m_Count(0)
		, m_Capacity(capacity)
		, m_pAllocator(pAllocator)
	{
		m_pData = static_cast<T*>(ALLOCATOR_SAFE(m_pAllocator)->Allocate((u32)sizeof(T) * m_Capacity));
		NTT_ASSERT_MSG(m_pData != nullptr, "Failed to allocate memory for Array.");
	}

	Array(const Array& other) = delete;

	Array(Array&& other) noexcept
		: m_pData(other.m_pData)
		, m_Count(other.m_Count)
		, m_Capacity(other.m_Capacity)
		, m_pAllocator(other.m_pAllocator)
	{
		other.m_pData	   = nullptr;
		other.m_Count	   = 0;
		other.m_Capacity   = 0;
		other.m_pAllocator = nullptr;
	}

	~Array()
	{
		if (m_pData != nullptr)
		{
			Clear(); // Call Clear to destruct elements before freeing memory
			NTT_ASSERT_MSG(ALLOCATOR_SAFE(m_pAllocator)->Free(m_pData, sizeof(T) * m_Capacity) == RESULT_SUCCESS,
						   "Failed to free memory for Array.");
		}
	}

	void operator=(const Array& other) = delete;
	void operator=(Array&& other) noexcept
	{
		if (this != &other)
		{
			NTT_ASSERT_MSG(ALLOCATOR_SAFE(m_pAllocator)->Free(m_pData, sizeof(T) * m_Capacity) == RESULT_SUCCESS,
						   "Failed to free memory for Array.");

			m_pData		 = other.m_pData;
			m_Count		 = other.m_Count;
			m_Capacity	 = other.m_Capacity;
			m_pAllocator = other.m_pAllocator;

			other.m_pData	   = nullptr;
			other.m_Count	   = 0;
			other.m_Capacity   = 0;
			other.m_pAllocator = nullptr;
		}
		return;
	}

	Result Append(T&& value)
	{
		if (m_Count > m_Capacity)
		{
			return RESULT_UNKNOWN;
		}

		if (m_Count == m_Capacity)
		{
			NTT_ASSERT_RESULT_SUCCESS(Resize(m_Capacity * 2));
		}

		m_pData[m_Count] = (T&&)value;
		++m_Count;
		return RESULT_SUCCESS;
	}

	Result Insert(T&& value, u32 index)
	{
		if (index > m_Count)
		{
			return RESULT_INDEX_OUT_OF_BOUNDS;
		}

		if (m_Count == m_Capacity)
		{
			NTT_ASSERT_RESULT_SUCCESS(Resize(m_Capacity * 2));
		}

		for (u32 i = m_Count; i > index; --i)
		{
			m_pData[i] = (T&&)m_pData[i - 1];
		}

		m_pData[index] = (T&&)value;
		++m_Count;
		return RESULT_SUCCESS;
	}

	Result Resize(u32 newCapacity)
	{
		if (newCapacity <= m_Capacity)
		{
			return RESULT_NEW_CAPACITY_TOO_SMALL;
		}

		T* pNewData = static_cast<T*>(ALLOCATOR_SAFE(m_pAllocator)->Allocate(sizeof(T) * newCapacity));

		if (pNewData == nullptr)
		{
			return RESULT_OUT_OF_MEMORY;
		}

		for (u32 i = 0; i < m_Count; ++i)
		{
			pNewData[i] = (T&&)m_pData[i];
		}

		NTT_ASSERT_MSG(ALLOCATOR_SAFE(m_pAllocator)->Free(m_pData, sizeof(T) * m_Capacity) == RESULT_SUCCESS,
					   "Failed to free memory for Array.");

		m_pData	   = pNewData;
		m_Capacity = newCapacity;
		return RESULT_SUCCESS;
	}

	T& operator[](u32 index)
	{
		NTT_ASSERT_MSG(index < m_Count, "Index out of bounds.");
		return m_pData[index];
	}

	inline u32 GetCount() const
	{
		return m_Count;
	}

	inline u32 GetCapacity() const
	{
		return m_Capacity;
	}

	Result Clear()
	{
		for (u32 i = 0; i < m_Count; ++i)
		{
			m_pData[i].~T(); // Call the destructor for each element
		}

		m_Count = 0;
		return RESULT_SUCCESS;
	}

	i32 FindIndex(Predicate predicate) const
	{
		for (u32 i = 0; i < m_Count; ++i)
		{
			if (predicate(m_pData[i]))
			{
				return i;
			}
		}
		return static_cast<i32>(-1); // Return -1 if not found
	}

	bool IsEmpty() const
	{
		return m_Count == 0;
	}

	bool Any(Predicate predicate) const
	{
		for (u32 i = 0; i < m_Count; ++i)
		{
			if (predicate(m_pData[i]))
			{
				return true;
			}
		}
		return false;
	}

	bool All(Predicate predicate) const
	{
		for (u32 i = 0; i < m_Count; ++i)
		{
			if (!predicate(m_pData[i]))
			{
				return false;
			}
		}
		return true;
	}

private:
	T*			m_pData		 = nullptr;
	u32			m_Count		 = 0;
	u32			m_Capacity	 = 0;
	IAllocator* m_pAllocator = nullptr;
};

} // namespace ntt

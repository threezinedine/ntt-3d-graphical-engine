#pragma once

#include "alloc/alloc.h"
#include "common.h"
#include "utils/utils.h"

#define NTT_ARRAY_DEFAULT_CAPACITY 4

namespace ntt {

template <typename T>
class Array
{
public:
	typedef bool (*Predicate)(const T&);

public:
	struct Iterator
	{
		Iterator(T* pCurrent)
			: m_pCurrent(pCurrent)
		{
		}

		Iterator(const Iterator& other)
			: m_pCurrent(other.m_pCurrent)
		{
		}

		Iterator(Iterator&& other) noexcept
		{
			m_pCurrent		 = other.m_pCurrent;
			other.m_pCurrent = nullptr;
		}

		void operator=(const Iterator& other)
		{
			m_pCurrent = other.m_pCurrent;
		}

		void operator=(Iterator&& other) noexcept
		{
			m_pCurrent		 = other.m_pCurrent;
			other.m_pCurrent = nullptr;
		}

		inline T& operator*() const
		{
			return *m_pCurrent;
		}

		inline Iterator& operator++()
		{
			++m_pCurrent;
			return *this;
		}

		inline bool operator!=(const Iterator& other) const
		{
			return m_pCurrent != other.m_pCurrent;
		}

		inline bool operator==(const Iterator& other) const
		{
			return m_pCurrent == other.m_pCurrent;
		}

	public:
		friend class Array<T>;

	private:
		T* m_pCurrent;
	};

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

	T& operator[](u32 index)
	{
		NTT_ASSERT_MSG(index < m_Count, "Index out of bounds.");
		return m_pData[index];
	}

	const T& operator[](u32 index) const
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

	inline Iterator end()
	{
		return Iterator(m_pData + m_Count);
	}

	inline Iterator end() const
	{
		return Iterator(m_pData + m_Count);
	}

	inline Iterator begin()
	{
		return Iterator(m_pData);
	}

	inline Iterator begin() const
	{
		return Iterator(m_pData);
	}

	inline bool IsEmpty() const
	{
		return m_Count == 0;
	}

	Result Resize(u32 newCapacity);
	Result Clear();

	Result Append(T&& value);
	Result Insert(T&& value, u32 index);
	Result Insert(T&& value, Iterator iter);

	Result Remove(u32 index);
	Result Remove(Iterator iter);

	i32		 FindIndex(Predicate predicate) const;
	Iterator Find(Predicate predicate);
	bool	 Any(Predicate predicate) const;
	bool	 All(Predicate predicate) const;

private:
	T*			m_pData		 = nullptr;
	u32			m_Count		 = 0;
	u32			m_Capacity	 = 0;
	IAllocator* m_pAllocator = nullptr;
};

template <typename T>
Result Array<T>::Append(T&& value)
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

template <typename T>
Result Array<T>::Insert(T&& value, typename Array<T>::Iterator iter)
{
	u32 index = static_cast<u32>(iter.m_pCurrent - m_pData);
	return Insert((T&&)value, index);
}

template <typename T>
Result Array<T>::Remove(u32 index)
{
	if (index >= m_Count)
	{
		return RESULT_INDEX_OUT_OF_BOUNDS;
	}

	for (u32 i = index; i < m_Count - 1; ++i)
	{
		m_pData[i] = (T&&)m_pData[i + 1];
	}

	--m_Count;
	return RESULT_SUCCESS;
}

template <typename T>
Result Array<T>::Remove(typename Array<T>::Iterator iter)
{
	u32 index = static_cast<u32>(iter.m_pCurrent - m_pData);
	return Remove(index);
}

template <typename T>
Result Array<T>::Insert(T&& value, u32 index)
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
		MemSet(&m_pData[i - 1], 0, sizeof(T)); // Clear the moved-from element
	}

	m_pData[index] = (T&&)value;
	++m_Count;
	return RESULT_SUCCESS;
}

template <typename T>
Result Array<T>::Resize(u32 newCapacity)
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

template <typename T>
Result Array<T>::Clear()
{
	for (u32 i = 0; i < m_Count; ++i)
	{
		m_pData[i].~T(); // Call the destructor for each element
	}

	m_Count = 0;
	return RESULT_SUCCESS;
}

template <typename T>
i32 Array<T>::FindIndex(Predicate predicate) const
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

template <typename T>
typename Array<T>::Iterator Array<T>::Find(Predicate predicate)
{
	for (u32 i = 0; i < m_Count; ++i)
	{
		if (predicate(m_pData[i]))
		{
			return Iterator(&m_pData[i]);
		}
	}
	return end(); // Return end iterator if not found
}

template <typename T>
bool Array<T>::Any(Predicate predicate) const
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

template <typename T>
bool Array<T>::All(Predicate predicate) const
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

} // namespace ntt

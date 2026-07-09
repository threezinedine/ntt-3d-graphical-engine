#pragma once

#include "array.h"
#include "common.h"
#include "stack.h"

namespace ntt {

template <typename DataType>
class Storage
{
public:
	struct Iterator
	{
		u32		 index;
		Storage* pStorage;

		friend class Storage<DataType>;

		Iterator(u32 index, Storage* pStorage)
			: index(index)
			, pStorage(pStorage)
		{
		}

		Iterator(const Iterator& other)
			: index(other.index)
			, pStorage(other.pStorage)
		{
		}

		Iterator(Iterator&& other) noexcept
		{
			index		   = other.index;
			pStorage	   = other.pStorage;
			other.index	   = 0;
			other.pStorage = nullptr;
		}

		void operator=(const Iterator& other)
		{
			index	 = other.index;
			pStorage = other.pStorage;
		}

		void operator=(Iterator&& other) noexcept
		{
			index		   = other.index;
			pStorage	   = other.pStorage;
			other.index	   = 0;
			other.pStorage = nullptr;
		}

		inline DataType& operator*() const
		{
			return pStorage->m_Nodes[index].data;
		}

		inline Iterator& operator++()
		{
			do
			{
				++index;
			} while (index < pStorage->m_CurrentIndex && !pStorage->m_Nodes[index].active);

			return *this;
		}

		bool operator!=(const Iterator& other) const
		{
			return index != other.index || pStorage != other.pStorage;
		}

		bool operator==(const Iterator& other) const
		{
			return index == other.index && pStorage == other.pStorage;
		}
	};

	friend struct Iterator;

public:
	Storage(u32 capacity = NTT_ARRAY_DEFAULT_CAPACITY, IAllocator* pAllocator = nullptr)
		: m_Nodes(capacity, pAllocator)
		, m_CurrentIndex(0)
		, m_FreeIndices(pAllocator)
	{
	}

	Storage(const Storage& other) = delete;
	Storage(Storage&& other)	  = delete;

	~Storage()
	{
		Clear();
	}

public:
	inline Result Clear()
	{
		NTT_ASSERT_RESULT_SUCCESS(m_FreeIndices.Clear());
		m_CurrentIndex = 0;

		return m_Nodes.Clear();
	}

	inline u32 GetCount() const
	{
		return m_CurrentIndex - m_FreeIndices.GetCount();
	}

	inline DataType* Get(u32 index)
	{
		if (index >= m_CurrentIndex || !m_Nodes[index].active)
		{
			return nullptr; // Return nullptr if index is out of bounds or inactive
		}

		return &m_Nodes[index].data;
	}

	Iterator begin()
	{
		u32 firstActiveIndex = 0;
		while (firstActiveIndex < m_CurrentIndex && !m_Nodes[firstActiveIndex].active)
		{
			++firstActiveIndex;
		}
		return Iterator(firstActiveIndex, this);
	}

	Iterator end()
	{
		return Iterator(m_CurrentIndex, this);
	}

public:
	u32 Add();

	Result Remove(u32 index);
	bool   IsActive(u32 index) const;

private:
	struct Node
	{
		DataType data;
		bool	 active;

		template <typename... Args>
		Node(Args&&... args)
			: data(static_cast<Args&&>(args)...)
			, active(true)
		{
		}
	};

	Array<Node> m_Nodes;
	u32			m_CurrentIndex;
	Stack<u32>	m_FreeIndices;
};

template <typename DataType>
u32 Storage<DataType>::Add()
{
	u32 index;
	if (!m_FreeIndices.IsEmpty())
	{
		index = m_FreeIndices.Top();
		m_FreeIndices.Pop();
	}
	else
	{
		index = m_CurrentIndex++;
	}

	if (index >= m_Nodes.GetCapacity())
	{
		m_Nodes.Resize(m_Nodes.GetCapacity() * 2);
	}

	m_Nodes[index].active = true;

	return index;
}

template <typename DataType>
Result Storage<DataType>::Remove(u32 index)
{
	if (index >= m_CurrentIndex)
	{
		return RESULT_INDEX_OUT_OF_BOUNDS;
	}

	if (!m_Nodes[index].active)
	{
		return RESULT_INACTIVE_STORAGE_INDEX; // Element is already inactive
	}

	m_Nodes[index].data.~DataType();
	m_Nodes[index].active = false;
	NTT_ASSERT_RESULT_SUCCESS(m_FreeIndices.Push(static_cast<u32&&>(index)));

	return RESULT_SUCCESS;
}

template <typename DataType>
bool Storage<DataType>::IsActive(u32 index) const
{
	return (index < m_CurrentIndex) && m_Nodes[index].active;
}

} // namespace ntt

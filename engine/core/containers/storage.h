#pragma once

#include "array.h"
#include "common.h"
#include "stack.h"

namespace ntt {

template <typename DataType>
class Storage
{
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

public:
	template <typename... Args>
	u32 Add(Args&&... args) noexcept;

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
template <typename... Args>
u32 Storage<DataType>::Add(Args&&... args) noexcept
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

	if (index < m_Nodes.GetCount())
	{
		m_Nodes[index].data	  = DataType(static_cast<Args&&>(args)...);
		m_Nodes[index].active = true;
	}
	else
	{
		m_Nodes.Emplace(static_cast<Args&&>(args)...);
	}

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

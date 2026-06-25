#pragma once

#include "alloc/alloc.h"
#include "common.h"

namespace ntt {

template <typename T>
class List
{
private:
	struct Node
	{
		T	  data;
		Node* pPrev;
		Node* pNext;
	};

public:
	List(IAllocator* pAllocator = nullptr)
		: m_pHead(nullptr)
		, m_pTail(nullptr)
		, m_Count(0)
		, m_pAllocator(pAllocator)
	{
	}

	List(const List& other) = delete;

	List(List&& other) noexcept
		: m_pHead(other.m_pHead)
		, m_pTail(other.m_pTail)
		, m_Count(other.m_Count)
		, m_pAllocator(other.m_pAllocator)
	{
		other.m_pHead	   = nullptr;
		other.m_pTail	   = nullptr;
		other.m_Count	   = 0;
		other.m_pAllocator = nullptr;
	}

	~List()
	{
		if (m_pHead != nullptr)
		{
			NTT_ASSERT_MSG(m_pTail != nullptr, "List is in an invalid state: m_pHead is not null but m_pTail is null.");
			NTT_ASSERT(Clear() == RESULT_SUCCESS);
		}
	}

	Result Clear()
	{
		Node* pCurrent = m_pHead;

		while (pCurrent != nullptr)
		{
			Node* pNext = pCurrent->pNext;
			pCurrent->~Node(); // Call the destructor for the node
			NTT_ASSERT_RESULT_SUCCESS(ALLOCATOR_SAFE(m_pAllocator)->Free(pCurrent, sizeof(Node)));
			pCurrent = pNext;
		}

		m_pHead = nullptr;
		m_pTail = nullptr;
		m_Count = 0;

		return RESULT_SUCCESS;
	}

	Result Append(T&& value)
	{
		Node* pNewNode = static_cast<Node*>(ALLOCATOR_SAFE(m_pAllocator)->Allocate(sizeof(Node)));

		if (pNewNode == nullptr)
		{
			return RESULT_OUT_OF_MEMORY;
		}

		pNewNode->data	= static_cast<T&&>(value);
		pNewNode->pPrev = m_pTail;
		pNewNode->pNext = nullptr;

		if (m_pTail != nullptr)
		{
			m_pTail->pNext = pNewNode;
		}
		else
		{
			m_pHead = pNewNode;
		}

		m_pTail = pNewNode;
		m_Count++;

		return RESULT_SUCCESS;
	}

	inline u32 GetCount() const
	{
		return m_Count;
	}

private:
	Node*		m_pHead;
	Node*		m_pTail;
	u32			m_Count;
	IAllocator* m_pAllocator;
};

} // namespace ntt

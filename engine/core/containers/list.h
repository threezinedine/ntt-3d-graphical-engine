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
	struct Iterator
	{
	public:
		Iterator(Node* pNode)
			: m_pCurrent(pNode)
		{
		}

		Iterator(const Iterator& other)
			: m_pCurrent(other.m_pCurrent)
		{
		}

		Iterator(Iterator&& other) noexcept
			: m_pCurrent(other.m_pCurrent)
		{
			other.m_pCurrent = nullptr;
		}

		void operator=(const Iterator& other)
		{
			if (this != &other)
			{
				m_pCurrent = other.m_pCurrent;
			}
		}

		void operator=(Iterator&& other) noexcept
		{
			if (this != &other)
			{
				m_pCurrent		 = other.m_pCurrent;
				other.m_pCurrent = nullptr;
			}
		}

		T& operator*() const
		{
			return m_pCurrent->data;
		}

		Iterator& operator++()
		{
			if (m_pCurrent != nullptr)
			{
				m_pCurrent = m_pCurrent->pNext;
			}
			return *this;
		}

		void operator++(int)
		{
			Iterator temp = *this;
			++(*this);
		}

		bool operator==(const Iterator& other) const
		{
			return m_pCurrent == other.m_pCurrent;
		}

		bool operator!=(const Iterator& other) const
		{
			return !(*this == other);
		}

	public:
		friend class List<T>;

	private:
		Node* m_pCurrent;
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

	inline u32 GetCount() const
	{
		return m_Count;
	}

	inline Iterator Begin() const
	{
		return Iterator(m_pHead);
	}

	inline Iterator End() const
	{
		return Iterator(nullptr);
	}

	Result Clear();
	Result Append(T&& value);
	Result InsertAfter(Iterator position, T&& value);
	Result InsertBefore(Iterator position, T&& value);
	Result Remove(Iterator position);
	T&	   operator[](u32 index);

private:
	Node*		m_pHead;
	Node*		m_pTail;
	u32			m_Count;
	IAllocator* m_pAllocator;
};

template <typename T>
Result List<T>::Append(T&& value)
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

template <typename T>
Result List<T>::Clear()
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

template <typename T>
Result List<T>::InsertAfter(typename List<T>::Iterator position, T&& value)
{
	Node* pCurrent = position.m_pCurrent;

	Node* pNewNode = (Node*)ALLOCATOR_SAFE(m_pAllocator)->Allocate(sizeof(Node));

	if (pNewNode == nullptr)
	{
		return RESULT_OUT_OF_MEMORY;
	}

	pNewNode->data	= static_cast<T&&>(value);
	pNewNode->pPrev = pCurrent;
	pNewNode->pNext = (pCurrent != nullptr) ? pCurrent->pNext : nullptr;

	if (pCurrent != nullptr)
	{
		if (pCurrent->pNext != nullptr)
		{
			pCurrent->pNext->pPrev = pNewNode;
		}
		else
		{
			m_pTail = pNewNode;
		}
		pCurrent->pNext = pNewNode;
	}
	else
	{
		if (m_pHead == nullptr)
		{
			m_pHead = pNewNode;
		}
		else
		{
			m_pTail->pNext = pNewNode;
		}
		m_pTail = pNewNode;
	}

	m_Count++;

	return RESULT_SUCCESS;
}

template <typename T>
Result List<T>::InsertBefore(typename List<T>::Iterator position, T&& value)
{
	Node* pCurrent = position.m_pCurrent;

	Node* pNewNode = (Node*)ALLOCATOR_SAFE(m_pAllocator)->Allocate(sizeof(Node));

	if (pNewNode == nullptr)
	{
		return RESULT_OUT_OF_MEMORY;
	}

	pNewNode->data	= static_cast<T&&>(value);
	pNewNode->pNext = pCurrent;
	pNewNode->pPrev = (pCurrent != nullptr) ? pCurrent->pPrev : nullptr;

	if (pCurrent != nullptr)
	{
		if (pCurrent->pPrev != nullptr)
		{
			pCurrent->pPrev->pNext = pNewNode;
		}
		else
		{
			m_pHead = pNewNode;
		}
		pCurrent->pPrev = pNewNode;
	}
	else
	{
		if (m_pTail == nullptr)
		{
			m_pTail = pNewNode;
		}
		else
		{
			m_pHead->pPrev = pNewNode;
		}
		m_pHead = pNewNode;
	}

	m_Count++;

	return RESULT_SUCCESS;
}

template <typename T>
Result List<T>::Remove(typename List<T>::Iterator position)
{
	Node* pCurrent = position.m_pCurrent;

	if (pCurrent == nullptr)
	{
		return RESULT_INVALID_ITERATOR;
	}

	if (pCurrent->pPrev != nullptr)
	{
		pCurrent->pPrev->pNext = pCurrent->pNext;
	}
	else
	{
		m_pHead = pCurrent->pNext;
	}

	if (pCurrent->pNext != nullptr)
	{
		pCurrent->pNext->pPrev = pCurrent->pPrev;
	}
	else
	{
		m_pTail = pCurrent->pPrev;
	}

	pCurrent->~Node(); // Call the destructor for the node
	NTT_ASSERT_RESULT_SUCCESS(ALLOCATOR_SAFE(m_pAllocator)->Free(pCurrent, sizeof(Node)));
	m_Count--;

	return RESULT_SUCCESS;
}

template <typename T>
T& List<T>::operator[](u32 index)
{
	NTT_ASSERT_MSG(index < m_Count, "Index out of bounds");

	Node* pCurrent = m_pHead;
	for (u32 i = 0; i < index; ++i)
	{
		pCurrent = pCurrent->pNext;
	}

	return pCurrent->data;
}

} // namespace ntt

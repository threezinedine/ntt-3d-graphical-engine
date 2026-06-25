#pragma once

#include "common.h"
#include "list.h"

namespace ntt {

template <typename T>
class Queue
{
public:
	Queue(IAllocator* pAllocator = nullptr)
		: m_List(pAllocator)
	{
	}

	Queue(const Queue& other) = delete;
	Queue(Queue&& other) noexcept
		: m_List((List<T>&&)other.m_List)
	{
	}

	~Queue() = default;

	Queue& operator=(const Queue& other) = delete;
	void   operator=(Queue&& other) noexcept
	{
		m_List = (List<T>&&)other.m_List;
	}

	inline bool IsEmpty() const
	{
		return m_List.GetCount() == 0;
	}

	Result Enqueue(T&& value);
	Result Dequeue();

	T& Front()
	{
		NTT_ASSERT_MSG(!IsEmpty(), "Queue is empty. Cannot access front element.");
		return m_List.Front();
	}

	const T& Front() const
	{
		NTT_ASSERT_MSG(!IsEmpty(), "Queue is empty. Cannot access front element.");
		return m_List.Front();
	}

	T& Back()
	{
		NTT_ASSERT_MSG(!IsEmpty(), "Queue is empty. Cannot access back element.");
		return m_List.Back();
	}

	const T& Back() const
	{
		NTT_ASSERT_MSG(!IsEmpty(), "Queue is empty. Cannot access back element.");
		return m_List.Back();
	}

private:
	List<T> m_List;
};

template <typename T>
Result Queue<T>::Enqueue(T&& value)
{
	return m_List.Append((T&&)value);
}

template <typename T>
Result Queue<T>::Dequeue()
{
	if (IsEmpty())
	{
		return RESULT_EMPTY_QUEUE;
	}
	return m_List.Remove(m_List.Begin());
}

} // namespace ntt

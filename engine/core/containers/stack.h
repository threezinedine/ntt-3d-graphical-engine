#pragma once

#include "common.h"
#include "list.h"

namespace ntt {

template <typename T>
class Stack
{
public:
	Stack(IAllocator* pAllocator = nullptr)
		: m_List(pAllocator)
	{
	}

	Stack(const Stack& other) = delete;
	Stack(Stack&& other) noexcept
		: m_List((List<T>&&)other.m_List)
	{
	}

	~Stack() = default;

	Stack& operator=(const Stack& other) = delete;
	void   operator=(Stack&& other) noexcept
	{
		m_List = (List<T>&&)other.m_List;
	}

	inline bool IsEmpty() const
	{
		return m_List.GetCount() == 0;
	}

	Result Push(const T& value) = delete;
	Result Push(T&& value) noexcept;
	Result Pop();

	T& Top()
	{
		NTT_ASSERT_MSG(!IsEmpty(), "Stack is empty. Cannot access top element.");
		return m_List.Back();
	}

	const T& Top() const
	{
		NTT_ASSERT_MSG(!IsEmpty(), "Stack is empty. Cannot access top element.");
		return m_List.Back();
	}

	inline u32 GetCount() const
	{
		return m_List.GetCount();
	}

	inline Result Clear()
	{
		return m_List.Clear();
	}

private:
	List<T> m_List;
};

template <typename T>
Result Stack<T>::Push(T&& value) noexcept
{
	return m_List.Append((T&&)value);
}

template <typename T>
Result Stack<T>::Pop()
{
	if (IsEmpty())
	{
		return RESULT_EMPTY_STACK;
	}
	return m_List.Remove(m_List.IterBack());
}

} // namespace ntt
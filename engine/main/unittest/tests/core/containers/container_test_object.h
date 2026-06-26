#pragma once

#include "core.h"
#include "utilities/utilities.h"

using namespace ntt;

namespace {

class TestObject
{
public:
	static i32 s_ConstructorCount;
	static i32 s_DestructorCount;
	static i32 s_CopyConstructorCount;
	static i32 s_MoveConstructorCount;

public:
	TestObject(i32 value = 0)
		: m_Value(value)
	{
		s_ConstructorCount++;
	}

	TestObject(const TestObject& other)
		: m_Value(other.m_Value)
	{
		s_CopyConstructorCount++;
	}

	TestObject(TestObject&& other) noexcept
		: m_Value(other.m_Value)
	{
		s_MoveConstructorCount++;
	}

	~TestObject()
	{
		s_DestructorCount++;
	}

	void operator=(TestObject&&) noexcept
	{
		s_MoveConstructorCount++;
	}

	void operator=(const TestObject&) = delete; // Disable copy assignment

	operator i32() const
	{
		return m_Value;
	}

private:
	i32 m_Value;
};

i32 TestObject::s_ConstructorCount	   = 0;
i32 TestObject::s_DestructorCount	   = 0;
i32 TestObject::s_CopyConstructorCount = 0;
i32 TestObject::s_MoveConstructorCount = 0;

} // namespace
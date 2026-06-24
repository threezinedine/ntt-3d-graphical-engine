#pragma once

#include "core.h"
#include <string.h> // TODO: Remove this include and use ntt::memset instead.

#define NTT_TEST_SUITE_NAME_LENGTH 50
#define NTT_UNITTEST_LINE_LENGTH   50

namespace ntt {

class TestSuite;

class Test
{
public:
	Test* m_NextTest = nullptr;

public:
	Test(const char* testName, bool useBeforeEachTestImpl = true);
	virtual ~Test();

public:
	void BeforeTest();
	void RunTest();
	void AfterTest();

protected:
	virtual void OnRunTestImpl() = 0;

protected:
	void AssignTestSuite(TestSuite* pTestSuite);

public:
	void ShowSuccess();
	void ShowFailure();

protected:
	TestSuite*	m_pTestSuite;
	const char* m_TestName;

public:
	inline const char* GetTestName() const
	{
		return m_TestName;
	}

	inline bool UseBeforeEachTestImpl() const
	{
		return m_UseBeforeEachTestImpl;
	}

protected:
	void SetFailure(const char* message, const char* fileName, u32 lineNumber);

private:
	bool		m_Success				= true;
	const char* m_FailureMessage		= nullptr;
	const char* m_FileName				= nullptr;
	u32			m_LineNumber			= 0;
	bool		m_UseBeforeEachTestImpl = true;
};

class TestSuite
{
public:
	TestSuite();
	virtual ~TestSuite();

public:
	void BeforeAllTests();
	void BeforeEachTest(Test* pTest);
	void AfterEachTest(Test* pTest);
	void AfterAllTests();

public:
	void AddTest(Test* pTest);

protected:
	virtual void OnBeforeAllTestsImpl();
	virtual void OnBeforeEachTestImpl();
	virtual void OnAfterEachTestImpl();
	virtual void OnAfterAllTestsImpl();

private:
	char m_TestSuiteName[NTT_TEST_SUITE_NAME_LENGTH + 1];

public:
	u32 m_TestCount	   = 0;
	u32 m_SuccessCount = 0;

public:
	Test* m_HeadTest = nullptr;
	Test* m_TailTest = nullptr;

public:
	inline void SetTestSuiteName(const char* testSuiteName)
	{
		memcpy(m_TestSuiteName, testSuiteName,
			   NTT_TEST_SUITE_NAME_LENGTH); // TODO: use ntt::copyString later
		m_TestSuiteName[NTT_TEST_SUITE_NAME_LENGTH] = '\0';
	}

	inline const char* GetTestSuiteName() const
	{
		return m_TestSuiteName;
	}

public:
	TestSuite* m_NextTestSuite = nullptr;
};

void centerText(char* pBuffer, u32 bufferSize, char* pText);

extern TestSuite* g_HeadTestSuite;
extern TestSuite* g_TailTestSuite;

} // namespace ntt

#define TEST_SUITE(testSuiteName) testSuiteName g_TestSuite_##testSuiteName##_instance;

#define TEST_CASE(testSuite, testName)			_TEST_CASE(testSuite, testName, true)
#define TEST_CASE_ISOLATED(testSuite, testName) _TEST_CASE(testSuite, testName, false)
#define _TEST_CASE(testSuite, testName, useBeforeEachTestImpl)                                                         \
	class testName : public ::ntt::Test                                                                                \
	{                                                                                                                  \
	public:                                                                                                            \
		testName()                                                                                                     \
			: ::ntt::Test(#testName, useBeforeEachTestImpl)                                                            \
		{                                                                                                              \
			AssignTestSuite(&g_TestSuite_##testSuite##_instance);                                                      \
			g_TestSuite_##testSuite##_instance.SetTestSuiteName(#testSuite);                                           \
		}                                                                                                              \
		~testName() override                                                                                           \
		{                                                                                                              \
		}                                                                                                              \
                                                                                                                       \
	protected:                                                                                                         \
		virtual void OnRunTestImpl() override;                                                                         \
	};                                                                                                                 \
	testName g_##testName##_instance;                                                                                  \
	void	 testName::OnRunTestImpl()

#define GET_SUITE(testSuite) g_TestSuite_##testSuite##_instance

#define RUN_ALL_TESTS()                                                                                                \
	do                                                                                                                 \
	{                                                                                                                  \
		if (g_HeadTestSuite == nullptr)                                                                                \
		{                                                                                                              \
			::ntt::setConsoleColor(::ntt::CONSOLE_COLOR_RED);                                                          \
			::ntt::print("No tests to run!\n");                                                                        \
		}                                                                                                              \
		else                                                                                                           \
		{                                                                                                              \
			::ntt::TestSuite* pCurrentTestSuite = ::ntt::g_HeadTestSuite;                                              \
			u32				  totalTestCount	= 0;                                                                   \
			u32				  totalSuccessCount = 0;                                                                   \
			while (pCurrentTestSuite != nullptr)                                                                       \
			{                                                                                                          \
				pCurrentTestSuite->BeforeAllTests();                                                                   \
				::ntt::Test* pCurrentTest = pCurrentTestSuite->m_HeadTest;                                             \
				while (pCurrentTest != nullptr)                                                                        \
				{                                                                                                      \
					pCurrentTest->BeforeTest();                                                                        \
					pCurrentTestSuite->BeforeEachTest(pCurrentTest);                                                   \
					pCurrentTest->RunTest();                                                                           \
					pCurrentTestSuite->AfterEachTest(pCurrentTest);                                                    \
					pCurrentTest->AfterTest();                                                                         \
					pCurrentTest = pCurrentTest->m_NextTest;                                                           \
				}                                                                                                      \
				totalTestCount += pCurrentTestSuite->m_TestCount;                                                      \
				totalSuccessCount += pCurrentTestSuite->m_SuccessCount;                                                \
				pCurrentTestSuite->AfterAllTests();                                                                    \
				pCurrentTestSuite = pCurrentTestSuite->m_NextTestSuite;                                                \
			}                                                                                                          \
			::ntt::print("\n");                                                                                        \
			if (totalTestCount == totalSuccessCount)                                                                   \
			{                                                                                                          \
				::ntt::setConsoleColor(::ntt::CONSOLE_COLOR_GREEN);                                                    \
			}                                                                                                          \
			else                                                                                                       \
			{                                                                                                          \
				::ntt::setConsoleColor(::ntt::CONSOLE_COLOR_RED);                                                      \
			}                                                                                                          \
			for (u32 i = 0; i < NTT_UNITTEST_LINE_LENGTH; ++i)                                                         \
			{                                                                                                          \
				::ntt::print("*");                                                                                     \
			}                                                                                                          \
			::ntt::print("\n");                                                                                        \
			char buffer[NTT_UNITTEST_LINE_LENGTH + 1];                                                                 \
			::ntt::format(buffer,                                                                                      \
						  NTT_UNITTEST_LINE_LENGTH + 1,                                                                \
						  "%u/%u tests passed. %u failed.",                                                            \
						  totalSuccessCount,                                                                           \
						  totalTestCount,                                                                              \
						  totalTestCount - totalSuccessCount);                                                         \
			char centeredBuffer[NTT_UNITTEST_LINE_LENGTH - 1];                                                         \
			::ntt::centerText(centeredBuffer, NTT_UNITTEST_LINE_LENGTH - 1, buffer);                                   \
			::ntt::print("*%s*\n", centeredBuffer);                                                                    \
			for (u32 i = 0; i < NTT_UNITTEST_LINE_LENGTH; ++i)                                                         \
			{                                                                                                          \
				::ntt::print("*");                                                                                     \
			}                                                                                                          \
			::ntt::print("\n");                                                                                        \
			::ntt::resetConsoleColor();                                                                                \
		}                                                                                                              \
	} while (0)

#define TEST_ASSERT(condition)                                                                                         \
	do                                                                                                                 \
	{                                                                                                                  \
		if (!(condition))                                                                                              \
		{                                                                                                              \
			SetFailure("Assertion failed: " #condition, __FILE__, __LINE__);                                           \
			return;                                                                                                    \
		}                                                                                                              \
	} while (0)

#define TEST_EQUAL(expected, actual)                                                                                   \
	do                                                                                                                 \
	{                                                                                                                  \
		if ((expected) != (actual))                                                                                    \
		{                                                                                                              \
			SetFailure("Assertion failed: " #expected " == " #actual, __FILE__, __LINE__);                             \
			return;                                                                                                    \
		}                                                                                                              \
	} while (0)

#define TEST_SUCCESS(condition)                                                                                        \
	do                                                                                                                 \
	{                                                                                                                  \
		if ((condition) != ::ntt::RESULT_SUCCESS)                                                                      \
		{                                                                                                              \
			SetFailure("Assertion failed: " #condition, __FILE__, __LINE__);                                           \
			return;                                                                                                    \
		}                                                                                                              \
	} while (0)

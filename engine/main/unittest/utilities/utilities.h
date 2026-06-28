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
	Test(const char* testName, bool useBeforeEachTestImpl = true, const char* fileName = nullptr, u32 lineNumber = 0);
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

public:
	void SetFailure(const char* message, const char* fileName, u32 lineNumber);

private:
	bool		m_Success				= true;
	const char* m_FailureMessage		= nullptr;
	const char* m_FileName				= nullptr;
	u32			m_LineNumber			= 0;
	bool		m_UseBeforeEachTestImpl = true;
	const char* m_TestFileName			= nullptr;
	u32			m_TestLineNumber		= 0;
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
	virtual void OnBeforeEachTestImpl(Test* pTest);
	virtual void OnAfterEachTestImpl(Test* pTest);
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
	class testSuite##_##testName : public ::ntt::Test                                                                  \
	{                                                                                                                  \
	public:                                                                                                            \
		testSuite##_##testName()                                                                                       \
			: ::ntt::Test(#testName, useBeforeEachTestImpl, __FILE__, __LINE__)                                        \
		{                                                                                                              \
			AssignTestSuite(&g_TestSuite_##testSuite##_instance);                                                      \
			g_TestSuite_##testSuite##_instance.SetTestSuiteName(#testSuite);                                           \
		}                                                                                                              \
		~testSuite##_##testName() override                                                                             \
		{                                                                                                              \
		}                                                                                                              \
                                                                                                                       \
	protected:                                                                                                         \
		virtual void OnRunTestImpl() override;                                                                         \
	};                                                                                                                 \
	testSuite##_##testName g_##testSuite##_##testName##_instance;                                                      \
	void				   testSuite##_##testName::OnRunTestImpl()

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

#define _TEST_ASSERT(pre, condition)                                                                                   \
	do                                                                                                                 \
	{                                                                                                                  \
		if (!(condition))                                                                                              \
		{                                                                                                              \
			pre SetFailure("Assertion failed: " #condition, __FILE__, __LINE__);                                       \
			return;                                                                                                    \
		}                                                                                                              \
	} while (0)

#define TEST_ASSERT(condition)	 _TEST_ASSERT(, condition)
#define W_TEST_ASSERT(condition) _TEST_ASSERT(pTest->, condition)

#define _TEST_EQUAL(pre, expected, actual)                                                                             \
	do                                                                                                                 \
	{                                                                                                                  \
		if ((expected) != (actual))                                                                                    \
		{                                                                                                              \
			pre SetFailure("Assertion failed: " #expected " == " #actual, __FILE__, __LINE__);                         \
			return;                                                                                                    \
		}                                                                                                              \
	} while (0)

#define TEST_EQUAL(expected, actual)   _TEST_EQUAL(, expected, actual)
#define W_TEST_EQUAL(expected, actual) _TEST_EQUAL(pTest->, expected, actual)

#define _TEST_NOT_EQUAL(pre, expected, actual)                                                                         \
	do                                                                                                                 \
	{                                                                                                                  \
		if ((expected) == (actual))                                                                                    \
		{                                                                                                              \
			pre SetFailure("Assertion failed: " #expected " != " #actual, __FILE__, __LINE__);                         \
			return;                                                                                                    \
		}                                                                                                              \
	} while (0)

#define TEST_NOT_EQUAL(expected, actual)   _TEST_NOT_EQUAL(, expected, actual)
#define W_TEST_NOT_EQUAL(expected, actual) _TEST_NOT_EQUAL(pTest->, expected, actual)

#define _TEST_SUCCESS(pre, condition)                                                                                  \
	do                                                                                                                 \
	{                                                                                                                  \
		Result _test_success_result = (condition);                                                                     \
		if (_test_success_result != ::ntt::RESULT_SUCCESS)                                                             \
		{                                                                                                              \
			char _test_success_buffer[1024];                                                                           \
			format(_test_success_buffer,                                                                               \
				   sizeof(_test_success_buffer),                                                                       \
				   "Assertion failed: " #condition " returned %s",                                                     \
				   ToString(_test_success_result));                                                                    \
			pre SetFailure(_test_success_buffer, __FILE__, __LINE__);                                                  \
			return;                                                                                                    \
		}                                                                                                              \
	} while (0)

#define ON_BEFORE_EACH() void OnBeforeEachTestImpl(Test* pTest) override
#define ON_AFTER_EACH()	 void OnAfterEachTestImpl(Test* pTest) override

#define TEST_SUCCESS(condition)	  _TEST_SUCCESS(, condition)
#define W_TEST_SUCCESS(condition) _TEST_SUCCESS(pTest->, condition)

#define _TEST_NOT_NULL(pre, condition)                                                                                 \
	do                                                                                                                 \
	{                                                                                                                  \
		if ((condition) == nullptr)                                                                                    \
		{                                                                                                              \
			pre SetFailure("Assertion failed: " #condition " != nullptr", __FILE__, __LINE__);                         \
			return;                                                                                                    \
		}                                                                                                              \
	} while (0)

#define TEST_NOT_NULL(condition)   _TEST_NOT_NULL(, condition)
#define W_TEST_NOT_NULL(condition) _TEST_NOT_NULL(pTest->, condition)
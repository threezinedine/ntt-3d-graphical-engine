#include "utilities.h"
#include "core.h"

#include <string.h> // TODO: Remove this include and use ntt::memset instead.

namespace ntt {

TestSuite* g_HeadTestSuite = nullptr;
TestSuite* g_TailTestSuite = nullptr;

Test::Test(const char* testName)
	: m_pTestSuite(nullptr)
	, m_TestName(testName)
{
}

void Test::AssignTestSuite(TestSuite* pTestSuite)
{
	// find if the test suite already exists in the linked list of test suites
	TestSuite* pExistedTestSuite = g_HeadTestSuite;
	while (pExistedTestSuite != nullptr)
	{
		if (pExistedTestSuite == pTestSuite)
		{
			break;
		}
		pExistedTestSuite = pExistedTestSuite->m_NextTestSuite;
	}

	if (pExistedTestSuite == nullptr)
	{
		if (g_HeadTestSuite == nullptr)
		{
			g_HeadTestSuite = pTestSuite;
			g_TailTestSuite = pTestSuite;
		}
		else
		{
			g_TailTestSuite->m_NextTestSuite = pTestSuite;
			g_TailTestSuite					 = pTestSuite;
		}
	}

	pTestSuite->AddTest(this);
	m_pTestSuite = pTestSuite;
}

Test::~Test()
{
}

void Test::BeforeTest()
{
	print("[ RUN      ] %s.%s\n", m_pTestSuite->GetTestSuiteName(), GetTestName());
}

void centerText(char* pBuffer, u32 bufferSize, char* pText)
{
	u32 textLength	  = (u32)strlen(pText); // TODO: Use ntt::strlen instead of std::strlen.
	u32 paddingLength = (bufferSize - textLength) / 2;
	memset(pBuffer, ' ', bufferSize - 1); // TODO: Use ntt::memset instead of std::memset.
	pBuffer[bufferSize - 1] = '\0';
	memcpy(pBuffer + paddingLength, pText, textLength); // TODO: Use ntt::memcpy instead of std::memcpy.
}

void Test::RunTest()
{
	OnRunTestImpl();
}

void Test::AfterTest()
{
	if (m_Success)
	{
		print("[       OK ] %s.%s\n", m_pTestSuite->GetTestSuiteName(), GetTestName());
		m_pTestSuite->m_SuccessCount++;
	}
	else
	{
		print("[  FAILED  ] %s.%s\n", m_pTestSuite->GetTestSuiteName(), GetTestName());
		print("\t%s:%u: Failure\n", m_FileName, m_LineNumber);
		print("\t%s\n", m_FailureMessage);
	}
}

void Test::SetFailure(const char* message, const char* fileName, u32 lineNumber)
{
	m_Success		 = false;
	m_FailureMessage = message;
	m_FileName		 = fileName;
	m_LineNumber	 = lineNumber;

	setConsoleColor(CONSOLE_COLOR_RED);
}

TestSuite::TestSuite()
	: m_TestSuiteName{0}
{
}

TestSuite::~TestSuite()
{
}

void TestSuite::BeforeAllTests()
{
	setConsoleColor(CONSOLE_COLOR_GREEN);
	print("[==========] Running %u tests from test suite: %s\n", m_TestCount, m_TestSuiteName);
	print("[----------] Global test environment setup\n");

	OnBeforeAllTestsImpl();
}

void TestSuite::BeforeEachTest(Test* pTest)
{
	NTT_UNUSED(pTest);
	setConsoleColor(CONSOLE_COLOR_GREEN);
	OnBeforeEachTestImpl();
}

void TestSuite::AfterEachTest(Test* pTest)
{
	NTT_UNUSED(pTest);
	OnAfterEachTestImpl();
}

void TestSuite::AfterAllTests()
{
	OnAfterAllTestsImpl();

	setConsoleColor(CONSOLE_COLOR_GREEN);
	print("[----------] %u tests from test suite: %s\n", m_TestCount, m_TestSuiteName);
	resetConsoleColor();
}

void TestSuite::OnBeforeAllTestsImpl()
{
}

void TestSuite::OnBeforeEachTestImpl()
{
}

void TestSuite::OnAfterEachTestImpl()
{
}

void TestSuite::OnAfterAllTestsImpl()
{
}

void TestSuite::AddTest(Test* pTest)
{
	if (m_HeadTest == nullptr)
	{
		m_HeadTest = pTest;
		m_TailTest = pTest;
	}
	else
	{
		m_TailTest->m_NextTest = pTest;
		m_TailTest			   = pTest;
	}
	m_TestCount++;
}

} // namespace ntt

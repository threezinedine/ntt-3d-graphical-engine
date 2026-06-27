#include "services.h"
#include "utilities/utilities.h"

using namespace ntt;

class LoggingTest : public TestSuite
{
	ON_BEFORE_EACH()
	{
		W_TEST_SUCCESS(GlobalAllocators::Register());
		W_TEST_SUCCESS(GlobalAllocators::Initialize());
	}

	ON_AFTER_EACH()
	{
		W_TEST_SUCCESS(GlobalAllocators::Shutdown());
		W_TEST_SUCCESS(GlobalAllocators::Unregister());
	}
};

TEST_SUITE(LoggingTest)

TEST_CASE(LoggingTest, LoggingMessageFormat)
{
	LoggingMessage message;
	message.type  = LOGGING_TYPE_OBJECT;
	message.level = LOGGING_LEVEL_INFO;
	message.file  = "test_file.cpp";
	message.line  = 42;

	// Format the message with a simple string
	TEST_SUCCESS(message.FormatMessage("%(type)"));
	TEST_EQUAL(message.finalMessage, "OBJECT");

	TEST_SUCCESS(message.FormatMessage("[%(type)]"));
	TEST_EQUAL(message.finalMessage, "[OBJECT]");

	TEST_SUCCESS(message.FormatMessage("%(type) - %(level)"));
	TEST_EQUAL(message.finalMessage, "OBJECT - INFO");

	TEST_SUCCESS(message.FormatMessage("%(not)"));
	TEST_EQUAL(message.finalMessage, "%(not)"); // Unrecognized keyword should remain unchanged

	TEST_SUCCESS(message.FormatMessage("%(type) - %(level) - %(file):%(line)"));
	TEST_EQUAL(message.finalMessage, "OBJECT - INFO - test_file.cpp:42");
}

TEST_CASE(LoggingTest, LoggingMessageFormatEmpty)
{
	LoggingMessage message;
	message.type  = LOGGING_TYPE_OBJECT;
	message.level = LOGGING_LEVEL_INFO;
	message.file  = "test_file.cpp";
	message.line  = 42;

	// Format the message with an empty string
	TEST_SUCCESS(message.FormatMessage(""));
	TEST_EQUAL(message.finalMessage.Length(), 0);
}

TEST_CASE(LoggingTest, LoggingMessageFormatWithAlignment)
{
	LoggingMessage message;
	message.type  = LOGGING_TYPE_OBJECT;
	message.level = LOGGING_LEVEL_INFO;
	message.file  = "test_file.cpp";
	message.line  = 42;

	// Format the message with alignment
	TEST_SUCCESS(message.FormatMessage("%(type):8!"));
	TEST_EQUAL(message.finalMessage, "  OBJECT");

	TEST_SUCCESS(message.FormatMessage("%(level):-10!"));
	TEST_EQUAL(message.finalMessage, "INFO      ");

	TEST_SUCCESS(message.FormatMessage("%(file):3!"));
	TEST_EQUAL(message.finalMessage, "test_file.cpp");

	TEST_SUCCESS(message.FormatMessage("%(file):-15!:%(line)"));
	TEST_EQUAL(message.finalMessage, "test_file.cpp  :42");

	TEST_SUCCESS(message.FormatMessage("%(line):5!"));
	TEST_EQUAL(message.finalMessage, "   42");
}
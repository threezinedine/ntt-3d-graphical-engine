#include "core.h"
#include "utilities/utilities.h"

using namespace ntt;

class ArgParserTest : public TestSuite
{
	ON_BEFORE_EACH()
	{
	}

	ON_AFTER_EACH()
	{
	}
};

TEST_SUITE(ArgParserTest)

TEST_CASE(ArgParserTest, ParseHelpOption)
{
	const char* argv[] = {"program_name", "--help"};
	i32			argc   = sizeof(argv) / sizeof(argv[0]);

	TEST_SUCCESS(ArgParser::Parse(argc, const_cast<char**>(argv)));
	TEST_EQUAL(ArgParser::GetArgInfo(ARG_OPTION_HELP)->value.boolValue, true);
}

TEST_CASE(ArgParserTest, ShortNameHelpOption)
{
	const char* argv[] = {"program_name", "-h"};
	i32			argc   = sizeof(argv) / sizeof(argv[0]);

	TEST_SUCCESS(ArgParser::Parse(argc, const_cast<char**>(argv)));
	TEST_EQUAL(ArgParser::GetArgInfo(ARG_OPTION_HELP)->value.boolValue, true);
}

TEST_CASE(ArgParserTest, NormaParsing)
{
	const char* argv[] = {"program_name"};
	i32			argc   = sizeof(argv) / sizeof(argv[0]);

	TEST_SUCCESS(ArgParser::Parse(argc, const_cast<char**>(argv)));
	TEST_EQUAL(ArgParser::GetArgInfo(ARG_OPTION_HELP)->value.boolValue, false);
}

TEST_CASE(ArgParserTest, ParseUnknownOption)
{
	const char* argv[] = {"program_name", "--unknown"};
	i32			argc   = sizeof(argv) / sizeof(argv[0]);

	Result result = ArgParser::Parse(argc, const_cast<char**>(argv));
	TEST_EQUAL(result, RESULT_INVALID_COMMAND_LINE_ARGUMENT);
}
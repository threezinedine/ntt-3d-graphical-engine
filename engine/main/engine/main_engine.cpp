#include "applications.h"

using namespace ntt;

int main(i32 argc, char** argv)
{
	NTT_ASSERT_RESULT_SUCCESS(ArgParser::Parse(argc, argv));

	if (ArgParser::GetArgInfo(ARG_OPTION_HELP)->value.boolValue)
	{
		NTT_ASSERT_RESULT_SUCCESS(ArgParser::PrintHelp());
		return 0;
	}

	Application app;
	NTT_ASSERT_RESULT_SUCCESS(app.Initialize(argc, argv));
	NTT_ASSERT_RESULT_SUCCESS(app.Run());
	NTT_ASSERT_RESULT_SUCCESS(app.Shutdown());

	return 0;
}
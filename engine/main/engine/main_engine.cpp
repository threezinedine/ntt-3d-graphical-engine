#include "core.h"

using namespace ntt;

int main(i32 argc, char** argv)
{
	g_Globals.argc = argc;
	g_Globals.argv = argv;

	NTT_ASSERT_RESULT_SUCCESS(GlobalAllocators::Register());
	NTT_ASSERT_RESULT_SUCCESS(GlobalAllocators::Initialize());

	Optional<void*> result = g_GlobalAllocators.pMalloc->Allocate(1024);
	g_GlobalAllocators.pMalloc->Free(result.value, 1000);

#define CONSOLE_COLOR_OPTION(option, console_value)                                                                    \
	setConsoleColor(CONSOLE_COLOR_##option);                                                                           \
	print("Line with %s!\n", ToString(CONSOLE_COLOR_##option));
#define CONSOLE_COLOR_OPTION_END(option)
#include "console_color.def"
#undef CONSOLE_COLOR_OPTION
#undef CONSOLE_COLOR_OPTION_END
	resetConsoleColor();
	print("Line with reset color!\n");

	NTT_ASSERT_RESULT_SUCCESS(GlobalAllocators::Shutdown());
	NTT_ASSERT_RESULT_SUCCESS(GlobalAllocators::Unregister());

	return 0;
}
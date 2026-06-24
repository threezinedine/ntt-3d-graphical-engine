#include "core.h"
#include "utilities/utilities.h"

using namespace ntt;

int main(i32 argc, char** argv)
{
	g_Globals.argc = argc;
	g_Globals.argv = argv;

	RUN_ALL_TESTS();

	return 0;
}
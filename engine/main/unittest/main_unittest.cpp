#include "core.h"

using namespace ntt;

int main(i32 argc, char** argv)
{
	g_Globals.argc = argc;
	g_Globals.argv = argv;

	print("Hello from the unit test!\n");

	return 0;
}
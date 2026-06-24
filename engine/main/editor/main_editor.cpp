#include "core.h"
#include <cstdio>

using namespace ntt;

int main(i32 argc, char** argv)
{
	g_Globals.argc = argc;
	g_Globals.argv = argv;

	print("Hello from the editor!\n");

	return 0;
}
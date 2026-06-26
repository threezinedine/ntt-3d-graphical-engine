#include "applications.h"

using namespace ntt;

int main(i32 argc, char** argv)
{
	Application app;
	NTT_ASSERT_RESULT_SUCCESS(app.Initialize(argc, argv));

	NTT_ASSERT_RESULT_SUCCESS(app.Run());

	NTT_ASSERT_RESULT_SUCCESS(app.Shutdown());

	return 0;
}
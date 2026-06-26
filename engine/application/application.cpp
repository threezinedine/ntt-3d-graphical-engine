#include "application.h"

namespace ntt {

Application::Application()
	: Object()
{
}

Application::~Application()
{
}

Result Application::Initialize(i32 argc, char** argv)
{
	g_Globals.argc = argc;
	g_Globals.argv = argv;

	NTT_ASSERT_RESULT_SUCCESS(GlobalAllocators::Register());
	NTT_ASSERT_RESULT_SUCCESS(GlobalAllocators::Initialize());

	return InitializeImpl();
}

Result Application::Run()
{
    print("Application is running...\n");
	return RESULT_SUCCESS;
}

Result Application::Shutdown()
{
	NTT_ASSERT_RESULT_SUCCESS(GlobalAllocators::Shutdown());
	NTT_ASSERT_RESULT_SUCCESS(GlobalAllocators::Unregister());
	return ShutdownImpl();
}

} // namespace ntt

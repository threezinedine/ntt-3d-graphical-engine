#include "applications.h"

namespace ntt {

static WindowID g_WindowID = INVALID_WINDOW_ID;

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
	NTT_ASSERT_RESULT_SUCCESS(IDSystem::Initialize());

	NTT_ASSERT_RESULT_SUCCESS(Logging::Initialize());
	NTT_ASSERT_RESULT_SUCCESS(RegisterApplicationType());

	NTT_ASSERT_RESULT_SUCCESS(SystemGlobals::Initialize());

	NTT_APPLICATION_INFO("Application initialized successfully.");

	NTT_ASSERT_RESULT_SUCCESS(SystemGlobals::pDisplaySystem->Initialize());
	g_WindowID = SystemGlobals::pDisplaySystem->CreateWindow(800, 600, "NTT Application");

	if (g_WindowID == INVALID_WINDOW_ID)
	{
		NTT_APPLICATION_ERROR("Failed to create window. Invalid window ID.");
		return RESULT_UNKNOWN;
	}

	m_pEcs = MakeScope<ECS>(g_GlobalAllocators.pMalloc);
	NTT_ASSERT_RESULT_SUCCESS(m_pEcs->Initialize());

	return InitializeImpl();
}

Result Application::Run()
{
	if (g_WindowID == INVALID_WINDOW_ID)
	{
		NTT_APPLICATION_ERROR("Invalid window ID. Cannot run the application.");
		return RESULT_UNKNOWN;
	}

	while (!SystemGlobals::pDisplaySystem->ShouldCloseWindow(g_WindowID))
	{
		NTT_ASSERT_RESULT_SUCCESS(SystemGlobals::pDisplaySystem->OnBeginFrame());

		// Application logic goes here

		NTT_ASSERT_RESULT_SUCCESS(SystemGlobals::pDisplaySystem->OnEndFrame());
	}

	return RESULT_SUCCESS;
}

Result Application::Shutdown()
{
	NTT_ASSERT_RESULT_SUCCESS(m_pEcs->Shutdown());
	m_pEcs.Reset();

	NTT_ASSERT_RESULT_SUCCESS(SystemGlobals::pDisplaySystem->DestroyWindow(g_WindowID));
	NTT_ASSERT_RESULT_SUCCESS(SystemGlobals::pDisplaySystem->Shutdown());

	NTT_APPLICATION_INFO("Application shut down successfully.", 3);

	NTT_ASSERT_RESULT_SUCCESS(SystemGlobals::Shutdown());

	NTT_ASSERT_RESULT_SUCCESS(UnregisterApplicationType());
	NTT_ASSERT_RESULT_SUCCESS(Logging::Shutdown());

	NTT_ASSERT_RESULT_SUCCESS(IDSystem::Shutdown());
	NTT_ASSERT_RESULT_SUCCESS(GlobalAllocators::Shutdown());
	NTT_ASSERT_RESULT_SUCCESS(GlobalAllocators::Unregister());

	return ShutdownImpl();
}

} // namespace ntt

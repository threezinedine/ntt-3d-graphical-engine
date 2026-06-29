#include "applications.h"

#if NTT_PLATFORM_WEB
#include <emscripten/emscripten.h>
#endif // NTT_PLATFORM_WEB
namespace ntt {

static WindowID		   g_WindowID		 = INVALID_WINDOW_ID;
static MeshID		   g_MeshID			 = INVALID_MESH_ID;
static RenderContextID g_RenderContextID = INVALID_RENDER_CONTEXT_ID;

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

	if (NTT_ARG_BOOL(VERBOSE))
	{
		NTT_ASSERT_RESULT_SUCCESS(Logging::SetLogLevel(LOGGING_LEVEL_DEBUG));
	}

	NTT_ASSERT_RESULT_SUCCESS(RegisterApplicationType());

	NTT_ASSERT_RESULT_SUCCESS(SystemGlobals::Initialize());

	NTT_APPLICATION_INFO("Application initialized successfully.");

	g_WindowID = SystemGlobals::pDisplaySystem->CreateWindow(800, 600, "NTT Application");

	if (g_WindowID == INVALID_WINDOW_ID)
	{
		NTT_APPLICATION_ERROR("Failed to create window. Invalid window ID.");
		return RESULT_UNKNOWN;
	}

	g_RenderContextID = SystemGlobals::pRenderSystem->CreateRenderContext(g_WindowID);

	m_pEcs = MakeScope<ECS>(g_GlobalAllocators.pMalloc);
	NTT_ASSERT_RESULT_SUCCESS(m_pEcs->Initialize());

	Mesh mesh;
	mesh.vertices.Emplace(Vec3f{0.0f, 0.5f, 0.0f}, Vec2f{0.5f, 1.0f}, Color{1.0f, 0.0f, 0.0f, 1.0f});
	mesh.vertices.Emplace(Vec3f{0.5f, -0.5f, 0.0f}, Vec2f{1.0f, 0.0f}, Color{0.0f, 1.0f, 0.0f, 1.0f});
	mesh.vertices.Emplace(Vec3f{-0.5f, -0.5f, 0.0f}, Vec2f{0.0f, 0.0f}, Color{0.0f, 0.0f, 1.0f, 1.0f});
	g_MeshID = g_RenderGlobals.pMeshStorage->AddMesh(static_cast<Mesh&&>(mesh));

	return InitializeImpl();
}

Result Application::Update()
{
	NTT_ASSERT_RESULT_SUCCESS(SystemGlobals::pDisplaySystem->OnBeginFrame(g_WindowID));

	NTT_ASSERT_RESULT_SUCCESS(SystemGlobals::pRenderSystem->BeginRender(g_RenderContextID));

	NTT_ASSERT_RESULT_SUCCESS(g_RenderGlobals.pShaderStorage->UseShader(g_DefaultMeshShaderID));
	NTT_ASSERT_RESULT_SUCCESS(g_RenderGlobals.pMeshStorage->DrawMesh(g_MeshID));

	NTT_ASSERT_RESULT_SUCCESS(SystemGlobals::pRenderSystem->EndRender(g_RenderContextID));
	NTT_ASSERT_RESULT_SUCCESS(SystemGlobals::pRenderSystem->Present(g_RenderContextID));

	NTT_ASSERT_RESULT_SUCCESS(SystemGlobals::pDisplaySystem->OnEndFrame(g_WindowID));

	return RESULT_SUCCESS;
} // namespace ntt

Result Application::Run()
{
	if (g_WindowID == INVALID_WINDOW_ID)
	{
		NTT_APPLICATION_ERROR("Invalid window ID. Cannot run the application.");
		return RESULT_UNKNOWN;
	}

#if NTT_PLATFORM_WEB
	emscripten_set_main_loop_arg(
		[](void* arg) {
			Application* pApp = static_cast<Application*>(arg);
			NTT_ASSERT_RESULT_SUCCESS(pApp->Update());
		},
		this,
		0,
		1);
#else  // NTT_PLATFORM_WEB
	while (!SystemGlobals::pDisplaySystem->ShouldCloseWindow(g_WindowID))
	{
		NTT_ASSERT_RESULT_SUCCESS(Update());
	}
#endif // NTT_PLATFORM_WEB
	return RESULT_SUCCESS;
}

Result Application::Shutdown()
{
	NTT_ASSERT_RESULT_SUCCESS(m_pEcs->Shutdown());
	m_pEcs.Reset();

	NTT_ASSERT_RESULT_SUCCESS(SystemGlobals::pRenderSystem->DestroyRenderContext(g_RenderContextID));
	NTT_ASSERT_RESULT_SUCCESS(SystemGlobals::pDisplaySystem->DestroyWindow(g_WindowID));

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

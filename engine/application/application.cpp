#include "applications.h"

#if NTT_PLATFORM_WEB
#include <emscripten/emscripten.h>
#endif // NTT_PLATFORM_WEB
namespace ntt {

static WindowID		   g_WindowID		 = INVALID_WINDOW_ID;
static MeshID		   g_MeshID			 = INVALID_MESH_ID;
static RenderContextID g_RenderContextID = INVALID_RENDER_CONTEXT_ID;

// #if !NTT_PLATFORM_WEB
// static WindowID		   g_SecondWindowID			 = INVALID_WINDOW_ID;
// static MeshID		   g_SecondMeshID			 = INVALID_MESH_ID;
// static RenderContextID g_SecondRenderContextID	 = INVALID_RENDER_CONTEXT_ID;

// #endif // !NTT_PLATFORM_WEB

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

	NTT_SHADER_STORAGE->SetupDefaultShaders(g_RenderContextID);

	m_pEcs = MakeScope<ECS>(g_GlobalAllocators.pMalloc);
	NTT_ASSERT_RESULT_SUCCESS(m_pEcs->Initialize());

	Mesh mesh;
	mesh.vertices.Emplace(Vec3f{-0.5f, -0.5f, 0.0f}, Vec2f{0.5f, 1.0f}, Color{1.0f, 0.0f, 0.0f, 1.0f});
	mesh.vertices.Emplace(Vec3f{0.5f, -0.5f, 0.0f}, Vec2f{1.0f, 0.0f}, Color{0.0f, 1.0f, 0.0f, 1.0f});
	mesh.vertices.Emplace(Vec3f{0.0f, 0.5f, 0.0f}, Vec2f{0.0f, 0.0f}, Color{0.0f, 0.0f, 1.0f, 1.0f});
	g_MeshID = NTT_MESH_STORAGE->AddMesh(static_cast<Mesh&&>(mesh), g_RenderContextID);

#if NTT_DEBUG
	NTT_MESH_STORAGE->SetDebugLineWidth(g_MeshID, 5);
#endif // NTT_DEBUG

	// #if !NTT_PLATFORM_WEB

	// 	g_SecondWindowID = SystemGlobals::pDisplaySystem->CreateWindow(1920, 1080, "NTT Second Window");

	// 	if (g_SecondWindowID == INVALID_WINDOW_ID)
	// 	{
	// 		NTT_APPLICATION_ERROR("Failed to create second window. Invalid window ID.");
	// 		return RESULT_UNKNOWN;
	// 	}

	// 	g_SecondRenderContextID = SystemGlobals::pRenderSystem->CreateRenderContext(g_SecondWindowID);
	// 	NTT_SHADER_STORAGE->SetupDefaultShaders(g_SecondRenderContextID);

	// 	Mesh secondMesh;
	// 	secondMesh.vertices.Emplace(Vec3f{-0.5f, -0.5f, 0.0f}, Vec2f{0.5f, 1.0f}, Color{1.0f, 0.0f, 0.0f, 1.0f});
	// 	secondMesh.vertices.Emplace(Vec3f{0.5f, -0.5f, 0.0f}, Vec2f{1.0f, 0.0f}, Color{0.0f, 1.0f, 0.0f, 1.0f});
	// 	secondMesh.vertices.Emplace(Vec3f{0.0f, 0.5f, 0.0f}, Vec2f{0.0f, 0.0f}, Color{0.0f, 0.0f, 1.0f, 1.0f});
	// 	g_SecondMeshID = NTT_MESH_STORAGE->AddMesh(static_cast<Mesh&&>(secondMesh), g_SecondRenderContextID);

	// #if NTT_DEBUG
	// 	NTT_MESH_STORAGE->SetDebugLineWidth(g_SecondMeshID, 5);
	// #endif // NTT_DEBUG

	// #endif // !NTT_PLATFORM_WEB

	return InitializeImpl();
}

Result Application::Update()
{
	NTT_ASSERT_RESULT_SUCCESS(SystemGlobals::pDisplaySystem->OnBeginFrame(g_WindowID));

	NTT_ASSERT_RESULT_SUCCESS(SystemGlobals::pRenderSystem->BeginRender(g_RenderContextID));

	Vec4f transform{0.2f, 0.0f, 0.0f, 0.0f};

	if (NTT_MESH_STORAGE->SetUniformFloat4(g_MeshID, "uColor", Color{1.0f, 0.0f, 1.0f, 1.0f}) != RESULT_SUCCESS)
	{
		NTT_APPLICATION_WARN("Failed to set uniform 'uColor' for mesh ID: %u", g_MeshID);
	}

	if (NTT_MESH_STORAGE->SetUniformFloat4(g_MeshID, "uTransform", transform) != RESULT_SUCCESS)
	{
		NTT_APPLICATION_WARN("Failed to set uniform 'uTransform' for mesh ID: %u", g_MeshID);
	}

	NTT_ASSERT_RESULT_SUCCESS(NTT_MESH_STORAGE->DrawMesh(g_MeshID));

#if NTT_DEBUG
	if (NTT_MESH_STORAGE->SetDebugLineUniformFloat4(g_MeshID, "uTransform", transform) != RESULT_SUCCESS)
	{
		NTT_APPLICATION_WARN("Failed to set uniform 'uTransform' for debug line of mesh ID: %u", g_MeshID);
	}

	if (NTT_MESH_STORAGE->SetDebugLineUniformFloat4(g_MeshID, "uColor", Color{0.0f, 0.0f, 1.0f, 1.0f}) !=
		RESULT_SUCCESS)
	{
		NTT_APPLICATION_WARN("Failed to set uniform 'uColor' for debug line of mesh ID: %u", g_MeshID);
	}

	NTT_ASSERT_RESULT_SUCCESS(NTT_MESH_STORAGE->DrawDebugLine(g_MeshID));
#endif // NTT_DEBUG

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
	// #if !NTT_PLATFORM_WEB
	// 	NTT_ASSERT_RESULT_SUCCESS(NTT_MESH_STORAGE->RemoveMesh(g_SecondMeshID));
	// 	NTT_ASSERT_RESULT_SUCCESS(NTT_SHADER_STORAGE->RemoveDefaultShaders(g_SecondRenderContextID));
	// 	NTT_ASSERT_RESULT_SUCCESS(SystemGlobals::pRenderSystem->DestroyRenderContext(g_SecondRenderContextID));
	// 	NTT_ASSERT_RESULT_SUCCESS(SystemGlobals::pDisplaySystem->DestroyWindow(g_SecondWindowID));
	// #endif // !NTT_PLATFORM_WEB

	NTT_ASSERT_RESULT_SUCCESS(m_pEcs->Shutdown());
	m_pEcs.Reset();

	NTT_ASSERT_RESULT_SUCCESS(NTT_SHADER_STORAGE->RemoveDefaultShaders(g_RenderContextID));

	NTT_ASSERT_RESULT_SUCCESS(NTT_MESH_STORAGE->RemoveMesh(g_MeshID));

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

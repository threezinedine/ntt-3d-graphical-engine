#include "applications.h"

#if NTT_PLATFORM_WEB
#include <emscripten/emscripten.h>
#endif // NTT_PLATFORM_WEB
namespace ntt {

static WindowID g_WindowID = INVALID_WINDOW_ID;
static MeshID	g_MeshID   = INVALID_MESH_ID;
static ShaderID g_ShaderID = INVALID_SHADER_ID;

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

	if (ArgParser::GetArgInfo(ARG_OPTION_VERBOSE)->value.boolValue)
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

	m_pEcs = MakeScope<ECS>(g_GlobalAllocators.pMalloc);
	NTT_ASSERT_RESULT_SUCCESS(m_pEcs->Initialize());

	Mesh mesh;
	mesh.vertices.Emplace(Vec3f{0.0f, 0.5f, 0.0f}, Vec2f{0.5f, 1.0f}, Color{1.0f, 0.0f, 0.0f, 1.0f});
	mesh.vertices.Emplace(Vec3f{0.5f, -0.5f, 0.0f}, Vec2f{1.0f, 0.0f}, Color{0.0f, 1.0f, 0.0f, 1.0f});
	mesh.vertices.Emplace(Vec3f{-0.5f, -0.5f, 0.0f}, Vec2f{0.0f, 0.0f}, Color{0.0f, 0.0f, 1.0f, 1.0f});
	g_MeshID = g_RenderGlobals.pMeshStorage->AddMesh(static_cast<Mesh&&>(mesh));

	const char* vertexShaderSource = R"(
		#version 330 core
		layout(location = 0) in vec3 aPos;
		layout(location = 1) in vec2 aTexCoord;
		layout(location = 2) in vec4 aColor;

		out vec2 TexCoord;
		out vec4 VertexColor;

		void main()
		{
			gl_Position = vec4(aPos, 1.0);
			TexCoord = aTexCoord;
			VertexColor = aColor;
		}
	)";

	const char* fragmentShaderSource = R"(
		#version 330 core
		out vec4 FragColor;

		in vec2 TexCoord;
		in vec4 VertexColor;

		void main()
		{
			FragColor = VertexColor; // Use the vertex color for the fragment color
		}
	)";

	g_ShaderID = g_RenderGlobals.pShaderStorage->AddShader(vertexShaderSource, fragmentShaderSource);

	return InitializeImpl();
}

Result Application::Update()
{
	NTT_ASSERT_RESULT_SUCCESS(SystemGlobals::pDisplaySystem->OnBeginFrame(g_WindowID));

	NTT_ASSERT_RESULT_SUCCESS(SystemGlobals::pRenderSystem->BeginRender());

	NTT_ASSERT_RESULT_SUCCESS(g_RenderGlobals.pShaderStorage->UseShader(g_ShaderID));
	NTT_ASSERT_RESULT_SUCCESS(g_RenderGlobals.pMeshStorage->DrawMesh(g_MeshID));

	NTT_ASSERT_RESULT_SUCCESS(SystemGlobals::pRenderSystem->EndRender());
	NTT_ASSERT_RESULT_SUCCESS(SystemGlobals::pRenderSystem->Present());

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

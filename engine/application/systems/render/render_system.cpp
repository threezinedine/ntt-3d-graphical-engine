#include "render_system.h"
#include "drivers/ntt_opengl/ntt_opengl.h"
#include "render_driver.h"
#include "render_globals.h"

namespace ntt {

NTT_OBJECT_DERIVED_DEFINE(RenderSystem, System)

RenderSystem::RenderSystem()
{
}

RenderSystem::~RenderSystem()
{
}

Result RenderSystem::InitializeImpl()
{
	NTT_ASSERT_RESULT_SUCCESS(RegisterOpenGLDriver());

	NTT_ASSERT_RESULT_SUCCESS(g_RenderDriver.Initialize());

	NTT_ASSERT_RESULT_SUCCESS(RegisterOpenGLRenderer());

	if (g_RenderGlobals.pMeshStorage == nullptr)
	{
		NTT_RENDER_ERROR("Failed to initialize mesh storage.");
		return RESULT_REGISTER_OPENGL_RENDERER_FAILED;
	}

	NTT_ASSERT_RESULT_SUCCESS(g_RenderGlobals.pMeshStorage->Initialize());
	NTT_ASSERT_RESULT_SUCCESS(g_RenderGlobals.pShaderStorage->Initialize());

	return RESULT_SUCCESS;
}

Result RenderSystem::ShutdownImpl()
{
	NTT_ASSERT_RESULT_SUCCESS(g_RenderGlobals.pShaderStorage->Shutdown());
	NTT_ASSERT_RESULT_SUCCESS(g_RenderGlobals.pMeshStorage->Shutdown());

	NTT_ASSERT_RESULT_SUCCESS(g_RenderGlobals.Shutdown());
	NTT_ASSERT_RESULT_SUCCESS(g_RenderDriver.Shutdown());

	return RESULT_SUCCESS;
}

} // namespace ntt
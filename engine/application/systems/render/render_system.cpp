#include "render_system.h"
#include "drivers/ntt_opengl/ntt_opengl.h"
#include "render_driver.h"

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

	return RESULT_SUCCESS;
}

Result RenderSystem::ShutdownImpl()
{
	NTT_ASSERT_RESULT_SUCCESS(g_RenderDriver.Shutdown());

	return RESULT_SUCCESS;
}

} // namespace ntt
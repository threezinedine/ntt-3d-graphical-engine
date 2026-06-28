#include "display_system.h"

namespace ntt {

NTT_OBJECT_DERIVED_DEFINE(DisplaySystem, System)

DisplaySystem::DisplaySystem()
{
}

DisplaySystem::~DisplaySystem()
{
}

Result DisplaySystem::InitializeImpl()
{
	NTT_DISPLAY_INFO("Display system initialized successfully.");
	return RESULT_SUCCESS;
}

Result DisplaySystem::ShutdownImpl()
{
	NTT_DISPLAY_INFO("Display system shut down successfully.");
	return RESULT_SUCCESS;
}

} // namespace ntt
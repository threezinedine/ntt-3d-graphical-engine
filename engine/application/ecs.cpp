#include "ecs.h"
#include "systems/systems.h"

namespace ntt {

ECS::ECS()
{
}

ECS::~ECS()
{
}

Result ECS::Initialize()
{
	m_pSystems = MakeScope<Storage<SystemInfo>>(g_GlobalAllocators.pMalloc);
	return RESULT_SUCCESS;
}

Result ECS::Shutdown()
{
	m_pSystems.Reset();
	return RESULT_SUCCESS;
}

ECSSystemID ECS::RegisterSystem(System* pSystem)
{
	return m_pSystems->Add(SystemInfo{pSystem});
}

Result ECS::UnregisterSystem(ECSSystemID systemID)
{
	return m_pSystems->Remove(systemID);
}

} // namespace ntt

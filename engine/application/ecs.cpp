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
	ECSSystemID systemID	= m_pSystems->Add();
	SystemInfo* pSystemInfo = m_pSystems->Get(systemID);
	if (pSystemInfo == nullptr)
	{
		return INVALID_ECS_SYSTEM_ID;
	}
	pSystemInfo->pSystem = pSystem;
	return systemID;
}

Result ECS::UnregisterSystem(ECSSystemID systemID)
{
	return m_pSystems->Remove(systemID);
}

} // namespace ntt

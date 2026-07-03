#pragma once

#include "services.h"

namespace ntt {

class System;

typedef u32 ECSSystemID;
typedef u32 ECSComponentID;
typedef u32 ECSEntityID;

constexpr ECSSystemID	 INVALID_ECS_SYSTEM_ID	  = static_cast<u32>(-1);
constexpr ECSComponentID INVALID_ECS_COMPONENT_ID = static_cast<u32>(-1);
constexpr ECSEntityID	 INVALID_ECS_ENTITY_ID	  = static_cast<u32>(-1);

class ECS
{
public:
	ECS();
	ECS(const ECS&) = delete;
	ECS(ECS&&)		= delete;
	~ECS();

	ECS& operator=(const ECS&) = delete;
	ECS& operator=(ECS&&)	   = delete;

public:
	Result Initialize();
	Result Shutdown();

public:
	ECSSystemID RegisterSystem(System* pSystem);
	Result		UnregisterSystem(ECSSystemID systemID);

private:
	struct SystemInfo
	{
		System* pSystem;
	};

	Scope<Storage<SystemInfo>> m_pSystems;
};

} // namespace ntt

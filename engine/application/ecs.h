#pragma once

#include "services.h"

namespace ntt {

class System;

typedef u32 ECSSystemID;
typedef u32 ECSComponentID;
typedef u32 ECSEntityID;

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

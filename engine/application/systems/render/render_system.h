#pragma once

#include "systems/system.h"

namespace ntt {

class RenderSystem : public System
{
	NTT_OBJECT_DERIVED_DECLARE(RenderSystem, System)

public:
	RenderSystem();
	virtual ~RenderSystem();

protected:
	virtual Result InitializeImpl() override;
	virtual Result ShutdownImpl() override;
};

} // namespace ntt

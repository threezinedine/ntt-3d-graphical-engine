#pragma once

#include "systems/system.h"

namespace ntt {

class DisplaySystem : public System
{
	NTT_OBJECT_DERIVED_DECLARE(DisplaySystem, System)

public:
	DisplaySystem();
	virtual ~DisplaySystem();

protected:
	virtual Result InitializeImpl() override;
	virtual Result ShutdownImpl() override;
};

} // namespace ntt

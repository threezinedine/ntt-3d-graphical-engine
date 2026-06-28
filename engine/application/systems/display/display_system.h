#pragma once

#include "systems/system.h"

namespace ntt {

class DisplaySystem : public System
{
	NTT_OBJECT_DERIVED_DECLARE(DisplaySystem, System)

public:
	DisplaySystem();
	virtual ~DisplaySystem();
};

} // namespace ntt

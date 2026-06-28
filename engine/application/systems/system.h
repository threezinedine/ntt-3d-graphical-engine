#pragma once

#include "object.h"
#include "services.h"

namespace ntt {

class System : public Object
{
	NTT_OBJECT_DERIVED_DECLARE(System, Object)

public:
	System();
	virtual ~System();
};

} // namespace ntt

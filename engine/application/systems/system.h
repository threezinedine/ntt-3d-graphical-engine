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

public:
	Result Initialize();
	Result Shutdown();

protected:
	virtual Result InitializeImpl() = 0;
	virtual Result ShutdownImpl()	= 0;
};

} // namespace ntt

#pragma once

#include "services.h"

namespace ntt {

#define NTT_OBJECT_DEFINE()                                                                                            \
public:                                                                                                                \
	static u32 s_TypeId;                                                                                               \
	Result	   RegisterType()                                                                                          \
	{                                                                                                                  \
		s_TypeId = g_Services.pTypeRegistry->RegisterType(typeid(*this).name());                                       \
		return RESULT_SUCCESS;                                                                                         \
	}

/**
 * Base class for all objects inside the engine
 */
class Object
{
public:
	Object();
	virtual ~Object();

public:
	void Print()
	{
		print("Object at %p\n", this);
	}
};

} // namespace ntt

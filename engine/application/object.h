#pragma once

#include "services.h"

namespace ntt {

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

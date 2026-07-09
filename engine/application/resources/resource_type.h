#pragma once

#include "services.h"

namespace ntt {

enum ResourceType
{
#define RESOURCE_TYPE_DEF(name, type, extension) NTT_RESOURCE_TYPE_##name,
#include "resource_type.def"
#undef RESOURCE_TYPE_DEF
	NTT_RESOURCE_TYPE_COUNT
};

const char* ToString(ResourceType type);

} // namespace ntt

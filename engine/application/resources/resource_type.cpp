#include "resource_type.h"

namespace ntt {

const char* ToString(ResourceType type)
{
	switch (type)
	{
#define RESOURCE_TYPE_DEF(name, type, extension)                                                                       \
	case ResourceType::NTT_RESOURCE_TYPE_##name:                                                                       \
		return #name;
#include "resource_type.def"
#undef RESOURCE_TYPE_DEF
	default:
		return "Unknown";
	}
}

} // namespace ntt

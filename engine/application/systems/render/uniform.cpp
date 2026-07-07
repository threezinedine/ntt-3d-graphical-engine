#include "uniform.h"

namespace ntt {

const char* ToString(UniformType uniformType)
{
	switch (uniformType)
	{
#define UNIFORM_TYPE_DEF(type, name, uppercase, glType)                                                                \
	case UNIFORM_TYPE_##uppercase:                                                                                     \
		return #name;
#include "uniform_type.def"
#undef UNIFORM_TYPE_DEF

	default:
		break;
	}

	return "Unknown";
}

u32 GetUniformTypeSize(UniformType uniformType)
{
	switch (uniformType)
	{
#define UNIFORM_TYPE_DEF(type, name, uppercase, glType)                                                                \
	case UNIFORM_TYPE_##uppercase:                                                                                     \
		return sizeof(type);
#include "uniform_type.def"
#undef UNIFORM_TYPE_DEF
	default:
		break;
	}
	return 0;
}

} // namespace ntt

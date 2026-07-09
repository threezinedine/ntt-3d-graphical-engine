#pragma once

#include "services.h"

namespace ntt {

typedef u32 TextureID;

enum UniformType
{
#define UNIFORM_TYPE_DEF(type, name, uppercase, glType)			UNIFORM_TYPE_##uppercase,
#define UNIFORM_TYPE_SAMPLER_DEF(type, name, uppercase, glType) UNIFORM_TYPE_##uppercase,
#include "uniform_type.def"
#undef UNIFORM_TYPE_DEF
#undef UNIFORM_TYPE_SAMPLER_DEF
};

union UniformValue {
#define UNIFORM_TYPE_DEF(type, name, uppercase, glType)			type name;
#define UNIFORM_TYPE_SAMPLER_DEF(type, name, uppercase, glType) type name;
#include "uniform_type.def"
#undef UNIFORM_TYPE_DEF
#undef UNIFORM_TYPE_SAMPLER_DEF
};

struct Uniform
{
	String		  name;
	UniformType	  type;
	UniformValue  value;
	Pointer<void> pInternalData;
};

const char* ToString(UniformType uniformType);
u32			GetUniformTypeSize(UniformType uniformType);

} // namespace ntt

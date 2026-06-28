#pragma once

#include "common.h"
#include "containers/stack.h"
#include "smart_ptr/smart_ptr.h"

#if NTT_UNITTEST
#define MAX_ID_VALUE 100
#else // NTT_UNITTEST
#define MAX_ID_VALUE 10000
#endif // NTT_UNITTEST

namespace ntt {

enum IDType
{
#define ID_TYPE_OPTION(option)	   ID_TYPE_##option,
#define ID_TYPE_OPTION_END(option) ID_TYPE_##option
#include "id.def"
#undef ID_TYPE_OPTION
#undef ID_TYPE_OPTION_END
};

struct ID
{
	IDType type;
	u32	   value;

	bool operator==(const ID& other) const
	{
		return type == other.type && value == other.value;
	}

	bool operator!=(const ID& other) const
	{
		return !(*this == other);
	}
};

constexpr ID INVALID_ID = {ID_TYPE_COUNT, static_cast<u32>(-1)};

class IDSystem
{
public:
	static Result Initialize();
	static ID	  NewID(IDType type);
	static Result FreeID(ID id);
	static Result Shutdown();

private:
	static ID				 s_NextID[MAX_ID_VALUE];
	static u32				 s_Index;
	static Scope<Stack<u32>> s_pFreeIDs;
};

} // namespace ntt
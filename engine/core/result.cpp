#include "result.h"

namespace ntt {

const char* ToString(Result result)
{
	switch (result)
	{
#define RESULT_OPTION(option)                                                                                          \
	case RESULT_##option:                                                                                              \
		return #option;
#define RESULT_OPTION_END(option)                                                                                      \
	default:                                                                                                           \
		return "Unknown Result";
#include "result.def"
#undef RESULT_OPTION
#undef RESULT_OPTION_END
	}
}

} // namespace ntt
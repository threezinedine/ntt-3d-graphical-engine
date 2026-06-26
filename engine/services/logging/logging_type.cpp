#include "logging_type.h"

namespace ntt {

const char* ToString(LoggingType type)
{
	switch (type)
	{
#define LOGGING_TYPE_OPTION(option)                                                                                    \
	case LOGGING_TYPE_##option:                                                                                        \
		return #option;
#define LOGGING_TYPE_OPTION_END(option)                                                                                \
	default:                                                                                                           \
		return "Unknown";
#include "logging_type.def"
#undef LOGGING_TYPE_OPTION
#undef LOGGING_TYPE_OPTION_END
	}
}

} // namespace ntt

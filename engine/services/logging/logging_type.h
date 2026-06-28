#pragma once

#include "core.h"

namespace ntt {

enum LoggingType
{
#define LOGGING_TYPE_OPTION(option, abbrev) LOGGING_TYPE_##option,
#define LOGGING_TYPE_OPTION_END(option)		LOGGING_TYPE_##option
#include "logging_type.def"
#undef LOGGING_TYPE_OPTION
#undef LOGGING_TYPE_OPTION_END
};

const char* ToString(LoggingType type);

} // namespace ntt

#pragma once

#include "core.h"
#include "handlers/handlers.h"
#include "logging_level.h"
#include "logging_message.h"
#include "logging_type.h"
#include "smart_ptr/smart_ptr.h"

namespace ntt {

class Logging
{
public:
	static Result Initialize();
	static Result Log(LoggingType type, LoggingLevel level, const char* file, u32 line, const char* format, ...);
	static Result Shutdown();

private:
	static Scope<Array<Scope<Handler>>> s_pHandlers;
};

#define NTT_LOG(type, level, format, ...)                                                                              \
	NTT_ASSERT_RESULT_SUCCESS(Logging::Log(type, level, __FILE__, __LINE__, format, ##__VA_ARGS__))

#define NTT_LOG_TRACE(type, format, ...)   NTT_LOG(type, LOGGING_LEVEL_TRACE, format, ##__VA_ARGS__)
#define NTT_LOG_DEBUG(type, format, ...)   NTT_LOG(type, LOGGING_LEVEL_DEBUG, format, ##__VA_ARGS__)
#define NTT_LOG_INFO(type, format, ...)	   NTT_LOG(type, LOGGING_LEVEL_INFO, format, ##__VA_ARGS__)
#define NTT_LOG_WARNING(type, format, ...) NTT_LOG(type, LOGGING_LEVEL_WARNING, format, ##__VA_ARGS__)
#define NTT_LOG_ERROR(type, format, ...)   NTT_LOG(type, LOGGING_LEVEL_ERROR, format, ##__VA_ARGS__)
#define NTT_LOG_FATAL(type, format, ...)   NTT_LOG(type, LOGGING_LEVEL_FATAL, format, ##__VA_ARGS__)

#define NTT_CORE_TRACE(format, ...)	  NTT_LOG_TRACE(LOGGING_TYPE_CORE, format, ##__VA_ARGS__)
#define NTT_CORE_DEBUG(format, ...)	  NTT_LOG_DEBUG(LOGGING_TYPE_CORE, format, ##__VA_ARGS__)
#define NTT_CORE_INFO(format, ...)	  NTT_LOG_INFO(LOGGING_TYPE_CORE, format, ##__VA_ARGS__)
#define NTT_CORE_WARNING(format, ...) NTT_LOG_WARNING(LOGGING_TYPE_CORE, format, ##__VA_ARGS__)
#define NTT_CORE_ERROR(format, ...)	  NTT_LOG_ERROR(LOGGING_TYPE_CORE, format, ##__VA_ARGS__)
#define NTT_CORE_FATAL(format, ...)	  NTT_LOG_FATAL(LOGGING_TYPE_CORE, format, ##__VA_ARGS__)

#define NTT_APPLICATION_TRACE(format, ...)	 NTT_LOG_TRACE(LOGGING_TYPE_APPLICATION, format, ##__VA_ARGS__)
#define NTT_APPLICATION_DEBUG(format, ...)	 NTT_LOG_DEBUG(LOGGING_TYPE_APPLICATION, format, ##__VA_ARGS__)
#define NTT_APPLICATION_INFO(format, ...)	 NTT_LOG_INFO(LOGGING_TYPE_APPLICATION, format, ##__VA_ARGS__)
#define NTT_APPLICATION_WARNING(format, ...) NTT_LOG_WARNING(LOGGING_TYPE_APPLICATION, format, ##__VA_ARGS__)
#define NTT_APPLICATION_ERROR(format, ...)	 NTT_LOG_ERROR(LOGGING_TYPE_APPLICATION, format, ##__VA_ARGS__)
#define NTT_APPLICATION_FATAL(format, ...)	 NTT_LOG_FATAL(LOGGING_TYPE_APPLICATION, format, ##__VA_ARGS__)

} // namespace ntt

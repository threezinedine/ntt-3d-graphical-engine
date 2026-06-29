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

	static Result SetLogLevel(LoggingLevel level);

private:
	static Scope<Array<Scope<Handler>>> s_pHandlers;
};

#define NTT_LOG(type, level, _format, ...)                                                                             \
	do                                                                                                                 \
	{                                                                                                                  \
		char _log_buffer[1024];                                                                                        \
		format(_log_buffer, sizeof(_log_buffer), _format, ##__VA_ARGS__);                                              \
		NTT_ASSERT_RESULT_SUCCESS(Logging::Log(type, level, __FILE__, __LINE__, _log_buffer));                         \
	} while (0)

#define NTT_LOG_TRACE(type, format, ...) NTT_LOG(type, LOGGING_LEVEL_TRACE, format, ##__VA_ARGS__)
#define NTT_LOG_DEBUG(type, format, ...) NTT_LOG(type, LOGGING_LEVEL_DEBUG, format, ##__VA_ARGS__)
#define NTT_LOG_INFO(type, format, ...)	 NTT_LOG(type, LOGGING_LEVEL_INFO, format, ##__VA_ARGS__)
#define NTT_LOG_WARN(type, format, ...)	 NTT_LOG(type, LOGGING_LEVEL_WARN, format, ##__VA_ARGS__)
#define NTT_LOG_ERROR(type, format, ...) NTT_LOG(type, LOGGING_LEVEL_ERROR, format, ##__VA_ARGS__)
#define NTT_LOG_FATAL(type, format, ...) NTT_LOG(type, LOGGING_LEVEL_FATAL, format, ##__VA_ARGS__)

#define NTT_CORE_TRACE(format, ...) NTT_LOG_TRACE(LOGGING_TYPE_CORE, format, ##__VA_ARGS__)
#define NTT_CORE_DEBUG(format, ...) NTT_LOG_DEBUG(LOGGING_TYPE_CORE, format, ##__VA_ARGS__)
#define NTT_CORE_INFO(format, ...)	NTT_LOG_INFO(LOGGING_TYPE_CORE, format, ##__VA_ARGS__)
#define NTT_CORE_WARN(format, ...)	NTT_LOG_WARN(LOGGING_TYPE_CORE, format, ##__VA_ARGS__)
#define NTT_CORE_ERROR(format, ...) NTT_LOG_ERROR(LOGGING_TYPE_CORE, format, ##__VA_ARGS__)
#define NTT_CORE_FATAL(format, ...) NTT_LOG_FATAL(LOGGING_TYPE_CORE, format, ##__VA_ARGS__)

#define NTT_APPLICATION_TRACE(format, ...) NTT_LOG_TRACE(LOGGING_TYPE_APPLICATION, format, ##__VA_ARGS__)
#define NTT_APPLICATION_DEBUG(format, ...) NTT_LOG_DEBUG(LOGGING_TYPE_APPLICATION, format, ##__VA_ARGS__)
#define NTT_APPLICATION_INFO(format, ...)  NTT_LOG_INFO(LOGGING_TYPE_APPLICATION, format, ##__VA_ARGS__)
#define NTT_APPLICATION_WARN(format, ...)  NTT_LOG_WARN(LOGGING_TYPE_APPLICATION, format, ##__VA_ARGS__)
#define NTT_APPLICATION_ERROR(format, ...) NTT_LOG_ERROR(LOGGING_TYPE_APPLICATION, format, ##__VA_ARGS__)
#define NTT_APPLICATION_FATAL(format, ...) NTT_LOG_FATAL(LOGGING_TYPE_APPLICATION, format, ##__VA_ARGS__)

#define NTT_DISPLAY_TRACE(format, ...) NTT_LOG_TRACE(LOGGING_TYPE_DISPLAY, format, ##__VA_ARGS__)
#define NTT_DISPLAY_DEBUG(format, ...) NTT_LOG_DEBUG(LOGGING_TYPE_DISPLAY, format, ##__VA_ARGS__)
#define NTT_DISPLAY_INFO(format, ...)  NTT_LOG_INFO(LOGGING_TYPE_DISPLAY, format, ##__VA_ARGS__)
#define NTT_DISPLAY_WARN(format, ...)  NTT_LOG_WARN(LOGGING_TYPE_DISPLAY, format, ##__VA_ARGS__)
#define NTT_DISPLAY_ERROR(format, ...) NTT_LOG_ERROR(LOGGING_TYPE_DISPLAY, format, ##__VA_ARGS__)
#define NTT_DISPLAY_FATAL(format, ...) NTT_LOG_FATAL(LOGGING_TYPE_DISPLAY, format, ##__VA_ARGS__)

#define NTT_OBJECT_TRACE(format, ...) NTT_LOG_TRACE(LOGGING_TYPE_OBJECT, format, ##__VA_ARGS__)
#define NTT_OBJECT_DEBUG(format, ...) NTT_LOG_DEBUG(LOGGING_TYPE_OBJECT, format, ##__VA_ARGS__)
#define NTT_OBJECT_INFO(format, ...)  NTT_LOG_INFO(LOGGING_TYPE_OBJECT, format, ##__VA_ARGS__)
#define NTT_OBJECT_WARN(format, ...)  NTT_LOG_WARN(LOGGING_TYPE_OBJECT, format, ##__VA_ARGS__)
#define NTT_OBJECT_ERROR(format, ...) NTT_LOG_ERROR(LOGGING_TYPE_OBJECT, format, ##__VA_ARGS__)
#define NTT_OBJECT_FATAL(format, ...) NTT_LOG_FATAL(LOGGING_TYPE_OBJECT, format, ##__VA_ARGS__)

#define NTT_RENDER_TRACE(format, ...) NTT_LOG_TRACE(LOGGING_TYPE_RENDER, format, ##__VA_ARGS__)
#define NTT_RENDER_DEBUG(format, ...) NTT_LOG_DEBUG(LOGGING_TYPE_RENDER, format, ##__VA_ARGS__)
#define NTT_RENDER_INFO(format, ...)  NTT_LOG_INFO(LOGGING_TYPE_RENDER, format, ##__VA_ARGS__)
#define NTT_RENDER_WARN(format, ...)  NTT_LOG_WARN(LOGGING_TYPE_RENDER, format, ##__VA_ARGS__)
#define NTT_RENDER_ERROR(format, ...) NTT_LOG_ERROR(LOGGING_TYPE_RENDER, format, ##__VA_ARGS__)
#define NTT_RENDER_FATAL(format, ...) NTT_LOG_FATAL(LOGGING_TYPE_RENDER, format, ##__VA_ARGS__)

#define NTT_OPENGL_TRACE(format, ...) NTT_LOG_TRACE(LOGGING_TYPE_OPENGL, format, ##__VA_ARGS__)
#define NTT_OPENGL_DEBUG(format, ...) NTT_LOG_DEBUG(LOGGING_TYPE_OPENGL, format, ##__VA_ARGS__)
#define NTT_OPENGL_INFO(format, ...)  NTT_LOG_INFO(LOGGING_TYPE_OPENGL, format, ##__VA_ARGS__)
#define NTT_OPENGL_WARN(format, ...)  NTT_LOG_WARN(LOGGING_TYPE_OPENGL, format, ##__VA_ARGS__)
#define NTT_OPENGL_ERROR(format, ...) NTT_LOG_ERROR(LOGGING_TYPE_OPENGL, format, ##__VA_ARGS__)
#define NTT_OPENGL_FATAL(format, ...) NTT_LOG_FATAL(LOGGING_TYPE_OPENGL, format, ##__VA_ARGS__)

#define NTT_VULKAN_TRACE(format, ...) NTT_LOG_TRACE(LOGGING_TYPE_VULKAN, format, ##__VA_ARGS__)
#define NTT_VULKAN_DEBUG(format, ...) NTT_LOG_DEBUG(LOGGING_TYPE_VULKAN, format, ##__VA_ARGS__)
#define NTT_VULKAN_INFO(format, ...)  NTT_LOG_INFO(LOGGING_TYPE_VULKAN, format, ##__VA_ARGS__)
#define NTT_VULKAN_WARN(format, ...)  NTT_LOG_WARN(LOGGING_TYPE_VULKAN, format, ##__VA_ARGS__)
#define NTT_VULKAN_ERROR(format, ...) NTT_LOG_ERROR(LOGGING_TYPE_VULKAN, format, ##__VA_ARGS__)
#define NTT_VULKAN_FATAL(format, ...) NTT_LOG_FATAL(LOGGING_TYPE_VULKAN, format, ##__VA_ARGS__)

} // namespace ntt

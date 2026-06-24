#pragma once

#include "common.h"
#include "console.h"

#include "backtrace/backtrace.h"

#if NTT_PLATFORM_UNIX | NTT_PLATFORM_WEB

#include <signal.h>

#define debug_break() raise(SIGTRAP)

#elif NTT_PLATFORM_WINDOWS // NTT_PLATFORM_UNIX

#if NTT_MSVC
#define debug_break() __debugbreak()
#else // NTT_MSVC
#error "debug_break() is not implemented for this compiler (Windows only)."
#endif // NTT_MSVC

#else // NTT_PLATFORM_UNIX

#endif // NTT_PLATFORM_UNIX

#if NTT_ENABLE_ASSERTION

#define NTT_ASSERT(expr)                                                                                               \
	do                                                                                                                 \
	{                                                                                                                  \
		if (!(expr))                                                                                                   \
		{                                                                                                              \
			setConsoleColor(CONSOLE_COLOR_RED);                                                                        \
			print("Assertion failed: %s, file: %s, line: %d\n", #expr, __FILE__, __LINE__);                            \
			resetConsoleColor();                                                                                       \
			Backtrace backtrace;                                                                                       \
			backtrace.Capture();                                                                                       \
			backtrace.Print();                                                                                         \
			debug_break();                                                                                             \
		}                                                                                                              \
	} while (0)

#define NTT_ASSERT_MSG(expr, msg)                                                                                      \
	do                                                                                                                 \
	{                                                                                                                  \
		if (!(expr))                                                                                                   \
		{                                                                                                              \
			char buffer[1024];                                                                                         \
			format(buffer, sizeof(buffer), "(%s, %d): %s\n", __FILE__, __LINE__, msg);                                 \
			setConsoleColor(CONSOLE_COLOR_RED);                                                                        \
			print("%s", buffer);                                                                                       \
			resetConsoleColor();                                                                                       \
			Backtrace backtrace;                                                                                       \
			backtrace.Capture();                                                                                       \
			backtrace.Print();                                                                                         \
			debug_break();                                                                                             \
		}                                                                                                              \
	} while (0)

#define NTT_ASSERT_RESULT_SUCCESS(_res) NTT_ASSERT_MSG((_res) == RESULT_SUCCESS, "Result is not success.")

#else // NTT_ENABLE_ASSERTION

#define NTT_ASSERT(expr)				((void)0)
#define NTT_ASSERT_MSG(expr, msg)		((void)0)
#define NTT_ASSERT_RESULT_SUCCESS(_res) return (_res)

#endif // NTT_ENABLE_ASSERTION
#include "backtrace.h"
#include "console.h"
#include "globals.h"
#include <cstring>
#include <stdlib.h>

#if NTT_PLATFORM_UNIX
#include <execinfo.h>
#elif NTT_PLATFORM_WEB
#elif NTT_PLATFORM_WINDOWS
#include <Windows.h>
#elif NTT_PLATFORM_ANDROID
#error "Backtrace is not implemented for Android yet."
#elif NTT_PLATFORM_IOS
#error "Backtrace is not implemented for iOS yet."
#elif NTT_PLATFORM_MACOS
#error "Backtrace is not implemented for macOS yet."
#else
#error "Unsupported platform for backtrace."
#endif // NTT_PLATFORM_UNIX

namespace ntt {

extern char** g_argv;

Backtrace::Backtrace()
	: m_Depth(0)
{
	memset(m_Addresses, 0, sizeof(m_Addresses));
}

Backtrace::Backtrace(const Backtrace& other)
	: m_Depth(other.m_Depth)
{
	memcpy(m_Addresses, other.m_Addresses, sizeof(m_Addresses));
}

Backtrace::Backtrace(Backtrace&& other) noexcept
	: m_Depth(other.m_Depth)
{
	memcpy(m_Addresses, other.m_Addresses, sizeof(m_Addresses));
	other.m_Depth = 0;
	memset(other.m_Addresses, 0, sizeof(other.m_Addresses));
}

Backtrace::~Backtrace()
{
}

Result Backtrace::Capture()
{
#if NTT_PLATFORM_UNIX
	m_Depth = backtrace(m_Addresses, NTT_MAX_BACKTRACE_DEPTH);
#elif NTT_PLATFORM_WINDOWS
	// Windows implementation would go here
	// For example, using CaptureStackBackTrace function
	m_Depth = CaptureStackBackTrace(0, NTT_MAX_BACKTRACE_DEPTH, m_Addresses, nullptr);
#elif NTT_PLATFORM_WEB
	// Web implementation would go here
	// For example, using emscripten_get_callstack function
	m_Depth = 0; // Placeholder for actual implementation
#else
#error "Backtrace capture is not implemented for this platform."
#endif // NTT_PLATFORM_UNIX

	return RESULT_SUCCESS;
}

Result Backtrace::Print() const
{
#if NTT_PLATFORM_UNIX
	for (u32 i = 0; i < m_Depth; ++i)
	{
		char addr2lineCommand[256];
		format(
			addr2lineCommand, sizeof(addr2lineCommand), "addr2line -e %s -piC %p", g_Globals.argv[0], m_Addresses[i]);
		system(addr2lineCommand);
	}
#elif NTT_PLATFORM_WINDOWS // NTT_PLATFORM_UNIX
	debug_break();
#if NTT_DEBUG
#endif // NTT_DEBUG

#elif NTT_PLATFORM_WEB // NTT_PLATFORM_UNIX
	// Web implementation would go here
	// For example, using emscripten_get_callstack function

#else // NTT_PLATFORM_UNIX
#error "Backtrace print is not implemented for this platform."
#endif // NTT_PLATFORM_UNIX
	return RESULT_SUCCESS;
}

} // namespace ntt

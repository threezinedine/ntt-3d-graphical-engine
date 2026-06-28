#include "argparser.h"
#include "utils/utils.h"
#include <string.h>

namespace ntt {

ArgInfo ArgParser::m_ArgInfos[] = {
#define ARG_OPTION(type, name, long, short, description) {ARG_TYPE_##type, #name, long, short, description, {}},
#define ARG_OPTION_DEFAULT(type, name, long, short, description, default_value)                                        \
	{ARG_TYPE_##type, name, long, short, description, ArgValue{default_value}},
#include "arg_options.def"
#undef ARG_OPTION
#undef ARG_OPTION_DEFAULT
};

static i32	  g_argc;
static char** g_argv;

static bool HandleFlagOption(ArgInfo& argInfo, i32* pCurrentArgIndex);
static bool HandleOption(ArgInfo& argInfo, i32* pCurrentArgIndex);

Result ArgParser::Parse(i32 argc, char** argv)
{
	u32 argCount = sizeof(m_ArgInfos) / sizeof(ArgInfo);

	for (u32 argIndex = 0; argIndex < argCount; ++argIndex)
	{
		MemSet(&m_ArgInfos[argIndex].value, 0, sizeof(ArgValue));
	}

	if (argc < 2)
	{
		return RESULT_SUCCESS;
	}

	g_argc = argc;
	g_argv = argv;

	i32 argInputIndex = 1;
#if !NTT_UNITTEST
	char* currentArg = g_argv[argInputIndex];
#endif // !NTT_UNITTEST
	while (argInputIndex < g_argc)
	{
		bool matched = false;

		for (u32 argIndex = 0; argIndex < argCount; ++argIndex)
		{
			if (HandleOption(m_ArgInfos[argIndex], &argInputIndex))
			{
				matched = true;
				break;
			}
		}

		if (!matched)
		{
#if !NTT_UNITTEST
			setConsoleColor(CONSOLE_COLOR_RED);
			print("[ERROR] Unknown argument: %s\n", currentArg);
#endif // !NTT_UNITTEST
			return RESULT_INVALID_COMMAND_LINE_ARGUMENT;
		}
	}

	return RESULT_SUCCESS;
}

ArgInfo* ArgParser::GetArgInfo(ArgOption option)
{
	switch (option)
	{
#define ARG_OPTION(type, name, long, short, description)                                                               \
	case ARG_OPTION_##name:                                                                                            \
		return &m_ArgInfos[ARG_OPTION_##name];
#define ARG_OPTION_DEFAULT(type, name, long, short, description, default_value)                                        \
	case ARG_OPTION_##name:                                                                                            \
		return &m_ArgInfos[ARG_OPTION_##name];
#include "arg_options.def"
#undef ARG_OPTION
#undef ARG_OPTION_DEFAULT
	default:
		return nullptr;
	}
}

Result ArgParser::PrintHelp()
{
	print("Usage:\n");
	for (const ArgInfo& argInfo : m_ArgInfos)
	{
		if (argInfo.type == ARG_TYPE_FLAG)
		{
			print("  \"%s\", \"%s\": %s\n", argInfo.shortName, argInfo.longName, argInfo.description);
		}
		else if (argInfo.type == ARG_TYPE_OPTIONAL)
		{
			print("  \"%s\", \"%s\" <value>: %s\n", argInfo.shortName, argInfo.longName, argInfo.description);
		}
		else if (argInfo.type == ARG_TYPE_REQUIRED)
		{
			print("  \"%s\", \"%s\" <value>: %s\n", argInfo.shortName, argInfo.longName, argInfo.description);
		}
		else
		{
#if !NTT_UNITTEST
			setConsoleColor(CONSOLE_COLOR_RED);
			print("[ERROR] Unknown argument type for argument '%s'.\n", argInfo.name);
			resetConsoleColor();
			NTT_UNREACHABLE();
#endif
			return RESULT_UNKNOWN;
		}
	}

	return RESULT_SUCCESS;
}

static bool HandleOption(ArgInfo& argInfo, i32* pCurrentArgIndex)
{
	if (argInfo.type == ARG_TYPE_FLAG)
	{
		return HandleFlagOption(argInfo, pCurrentArgIndex);
	}

	return false;
}

static bool HandleFlagOption(ArgInfo& argInfo, i32* pCurrentArgIndex)
{
	if (strcmp(g_argv[*pCurrentArgIndex], argInfo.longName) == 0 ||
		strcmp(g_argv[*pCurrentArgIndex], argInfo.shortName) == 0)
	{
		argInfo.value.boolValue = true;
		++(*pCurrentArgIndex);
		return true;
	}
	return false;
}

} // namespace ntt

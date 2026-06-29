#pragma once

#include "common.h"

namespace ntt {

enum ArgType
{
#define ARG_TYPE_OPTION(name)	  ARG_TYPE_##name,
#define ARG_TYPE_OPTION_END(name) ARG_TYPE_##name,
#include "arg_type.def"
#undef ARG_TYPE_OPTION
#undef ARG_TYPE_OPTION_END
};

enum ArgOption
{
#define ARG_OPTION(type, name, long, short, description)						ARG_OPTION_##name,
#define ARG_OPTION_DEFAULT(type, name, long, short, description, default_value) ARG_OPTION_##name,
#include "arg_options.def"
#undef ARG_OPTION
#undef ARG_OPTION_DEFAULT
};

union ArgValue {
	bool   boolValue;
	i32	   intValue;
	char*  stringValue;
	f32	   floatValue;
	char** stringArrayValue;
};

struct ArgInfo
{
	ArgType		type;
	const char* name;
	const char* longName;
	const char* shortName;
	const char* description;
	ArgValue	value;
};

class ArgParser
{
public:
	static Result	Parse(i32 argc, char** argv);
	static ArgInfo* GetArgInfo(ArgOption option);
	static Result	PrintHelp();

private:
	static ArgInfo m_ArgInfos[];
};

#define NTT_ARG_BOOL(option) (::ntt::ArgParser::GetArgInfo(ARG_OPTION_##option)->value.boolValue)

} // namespace ntt

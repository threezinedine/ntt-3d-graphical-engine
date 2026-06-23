#pragma once

namespace ntt {

enum Result
{
#define RESULT_OPTION(option)	  RESULT_##option,
#define RESULT_OPTION_END(option) RESULT_##option
#include "result.inl"
#undef RESULT_OPTION
#undef RESULT_OPTION_END
};

const char* ToString(Result result);

} // namespace ntt
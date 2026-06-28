#pragma once

#include "services.h"

namespace ntt {

struct RenderDriver
{
	Result (*Initialize)();
	Result (*Shutdown)();
};

extern RenderDriver g_RenderDriver;

} // namespace ntt

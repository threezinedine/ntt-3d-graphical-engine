#pragma once
#include "core.h"
#include "systems/display/display_driver.h"

namespace ntt {

Result RegisterGLFWDisplayDriver();

extern void* g_DefaultGLFWDriverHandle;

} // namespace ntt

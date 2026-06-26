#pragma once

#include "common.h"

namespace ntt {

Result MemSet(void* ptr, u8 value, u32 size);
Result MemCopy(void* dest, const void* src, u32 size);

} // namespace ntt

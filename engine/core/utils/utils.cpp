#include "utils.h"
#include <string.h>

namespace ntt {

Result MemSet(void* ptr, u8 value, u32 size)
{
	memset(ptr, value, size);
	return RESULT_SUCCESS;
}

Result MemCopy(void* dest, const void* src, u32 size)
{
	memcpy(dest, src, size);
	return RESULT_SUCCESS;
}

} // namespace ntt
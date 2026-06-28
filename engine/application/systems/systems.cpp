#include "systems.h"

namespace ntt {

Result RegisterSystemTypes()
{
	NTT_ASSERT_RESULT_SUCCESS(System::RegisterType());
	NTT_ASSERT_RESULT_SUCCESS(DisplaySystem::RegisterType());

	return RESULT_SUCCESS;
}

Result UnregisterSystemTypes()
{
	NTT_ASSERT_RESULT_SUCCESS(DisplaySystem::UnregisterType());
	NTT_ASSERT_RESULT_SUCCESS(System::UnregisterType());

	return RESULT_SUCCESS;
}

} // namespace ntt

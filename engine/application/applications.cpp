#include "applications.h"

namespace ntt {

Result RegisterApplicationType()
{
	NTT_ASSERT_RESULT_SUCCESS(Object::RegisterType());
	NTT_ASSERT_RESULT_SUCCESS(RefCounted::RegisterType());

	return RESULT_SUCCESS;
}

Result UnregisterApplicationType()
{
	NTT_ASSERT_RESULT_SUCCESS(RefCounted::UnregisterType());
	NTT_ASSERT_RESULT_SUCCESS(Object::UnregisterType());

	return RESULT_SUCCESS;
}

} // namespace ntt

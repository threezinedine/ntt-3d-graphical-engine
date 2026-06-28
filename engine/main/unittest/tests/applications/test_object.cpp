#include "applications.h"
#include "utilities/utilities.h"

using namespace ntt;

class ObjectTest : public TestSuite
{
	ON_BEFORE_EACH()
	{
		W_TEST_SUCCESS(GlobalAllocators::Register());
		W_TEST_SUCCESS(GlobalAllocators::Initialize());
		W_TEST_SUCCESS(IDSystem::Initialize());
		W_TEST_SUCCESS(Logging::Initialize());
		W_TEST_SUCCESS(RegisterApplicationType());
	}

	ON_AFTER_EACH()
	{
		W_TEST_SUCCESS(UnregisterApplicationType());
		W_TEST_SUCCESS(Logging::Shutdown());
		W_TEST_SUCCESS(IDSystem::Shutdown());
		W_TEST_SUCCESS(GlobalAllocators::Shutdown());
		W_TEST_SUCCESS(GlobalAllocators::Unregister());
	}
};

TEST_SUITE(ObjectTest)

TEST_CASE(ObjectTest, ObjectTypeRegistration)
{
	Scope<RefCounted> refCounted = MakeScope<RefCounted>(g_GlobalAllocators.pMalloc);

	TEST_EQUAL(Object::HasInstance(refCounted.Get()), true);
}
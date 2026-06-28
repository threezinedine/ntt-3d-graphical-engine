#include "core.h"
#include "utilities/utilities.h"

using namespace ntt;

class IDTest : public TestSuite
{
	ON_BEFORE_EACH()
	{
		W_TEST_SUCCESS(GlobalAllocators::Register());
		W_TEST_SUCCESS(GlobalAllocators::Initialize());

		W_TEST_SUCCESS(IDSystem::Initialize());
	}

	ON_AFTER_EACH()
	{
		W_TEST_SUCCESS(IDSystem::Shutdown());

		W_TEST_SUCCESS(GlobalAllocators::Shutdown());
		W_TEST_SUCCESS(GlobalAllocators::Unregister());
	}
};

TEST_SUITE(IDTest)

TEST_CASE(IDTest, CreateAndFreeID)
{
	ID newID = IDSystem::NewID(ID_TYPE_OBJECT_TYPE);
	TEST_NOT_EQUAL(newID, INVALID_ID);

	Result freeResult = IDSystem::FreeID(newID);
	TEST_EQUAL(freeResult, RESULT_SUCCESS);
}

TEST_CASE(IDTest, RunOutOfIDs)
{
	// Create the maximum number of IDs
	for (u32 i = 0; i < MAX_ID_VALUE; ++i)
	{
		ID newID = IDSystem::NewID(ID_TYPE_OBJECT_TYPE);
		TEST_NOT_EQUAL(newID, INVALID_ID);
	}

	// Attempt to create one more ID, which should fail
	ID extraID = IDSystem::NewID(ID_TYPE_OBJECT_TYPE);
	TEST_EQUAL(extraID, INVALID_ID);
}

TEST_CASE(IDTest, FreeInvalidID)
{
	ID invalidID = {ID_TYPE_OBJECT_TYPE, MAX_ID_VALUE + 1}; // An ID that was never allocated
	TEST_EQUAL(IDSystem::FreeID(invalidID), RESULT_FREE_INVALID_ID);
}
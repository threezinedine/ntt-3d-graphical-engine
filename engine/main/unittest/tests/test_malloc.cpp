#include "core.h"
#include "utilities/utilities.h"

class MallocTest : public ::ntt::TestSuite
{
};

TEST_SUITE(MallocTest)

TEST_CASE(MallocTest, TestMallocAndFree)
{
}

TEST_CASE(MallocTest, FailedTest)
{
	TEST_ASSERT(1 == 2);
}
#include "id.h"

namespace ntt {

ID				  IDSystem::s_NextID[MAX_ID_VALUE];
u32				  IDSystem::s_Index = 0;
Scope<Stack<u32>> IDSystem::s_pFreeIDs;

Result IDSystem::Initialize()
{
	for (u32 i = 0; i < MAX_ID_VALUE; ++i)
	{
		s_NextID[i] = INVALID_ID;
	}

	s_Index	   = 0;
	s_pFreeIDs = MakeScope<Stack<u32>>(g_GlobalAllocators.pMalloc);

	return RESULT_SUCCESS;
}

ID IDSystem::NewID(IDType type)
{
	ID newID = INVALID_ID;
	if (s_pFreeIDs->IsEmpty())
	{
		if (s_Index >= MAX_ID_VALUE)
		{
			return INVALID_ID;
		}

		newID.type	= type;
		newID.value = s_Index++;
	}
	else
	{
		u32 freeIndex = s_pFreeIDs->Top();
		s_pFreeIDs->Pop();

		newID.type	= type;
		newID.value = freeIndex;
	}
	s_NextID[newID.value].type	= type;
	s_NextID[newID.value].value = newID.value;

	return newID;
}

Result IDSystem::FreeID(ID id)
{
	if (id.value >= MAX_ID_VALUE || s_NextID[id.value] == INVALID_ID)
	{
		return RESULT_FREE_INVALID_ID;
	}

	if (id == INVALID_ID)
	{
		return RESULT_FREE_INVALID_ID;
	}

	s_pFreeIDs->Push(static_cast<u32&&>(id.value));
	s_NextID[id.value] = INVALID_ID;

	return RESULT_SUCCESS;
}

Result IDSystem::Shutdown()
{
	s_pFreeIDs.Reset();
	return RESULT_SUCCESS;
}

} // namespace ntt

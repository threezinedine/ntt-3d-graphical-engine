#pragma once

#include "services.h"

namespace ntt {

struct IDInfo
{
	ID		id;
	IDInfo* pParentInfo;
};

#define NTT_OBJECT_TYPE_CHECK_METHOD(className)                                                                        \
	static bool HasInstance(Object* pObject)                                                                           \
	{                                                                                                                  \
		if (pObject == nullptr)                                                                                        \
		{                                                                                                              \
			return false;                                                                                              \
		}                                                                                                              \
		IDInfo* pCurrentInfo = &s_IDInfo;                                                                              \
		while (pCurrentInfo != nullptr)                                                                                \
		{                                                                                                              \
			if (pCurrentInfo->id == pObject->s_IDInfo.id)                                                              \
			{                                                                                                          \
				return true;                                                                                           \
			}                                                                                                          \
			pCurrentInfo = pCurrentInfo->pParentInfo;                                                                  \
		}                                                                                                              \
		return false;                                                                                                  \
	}

#define NTT_OBJECT_BASE_DECLARE(className)                                                                             \
public:                                                                                                                \
	static IDInfo s_IDInfo;                                                                                            \
	static Result RegisterType()                                                                                       \
	{                                                                                                                  \
		s_IDInfo.id			 = IDSystem::NewID(ID_TYPE_OBJECT_TYPE);                                                   \
		s_IDInfo.pParentInfo = nullptr;                                                                                \
		if (s_IDInfo.id == INVALID_ID)                                                                                 \
		{                                                                                                              \
			return RESULT_OUT_OF_IDS;                                                                                  \
		}                                                                                                              \
		return RESULT_SUCCESS;                                                                                         \
	}                                                                                                                  \
	static Result UnregisterType()                                                                                     \
	{                                                                                                                  \
		NTT_ASSERT_RESULT_SUCCESS(IDSystem::FreeID(s_IDInfo.id));                                                      \
		s_IDInfo.pParentInfo = nullptr;                                                                                \
		return RESULT_SUCCESS;                                                                                         \
	}                                                                                                                  \
	NTT_OBJECT_TYPE_CHECK_METHOD(className)

#define NTT_OBJECT_BASE_DEFINE(className) IDInfo className::s_IDInfo = {INVALID_ID, nullptr};

#define NTT_OBJECT_DERIVED_DECLARE(className, baseClassName)                                                           \
public:                                                                                                                \
	static IDInfo s_IDInfo;                                                                                            \
	static Result RegisterType()                                                                                       \
	{                                                                                                                  \
		s_IDInfo.id			 = IDSystem::NewID(ID_TYPE_OBJECT_TYPE);                                                   \
		s_IDInfo.pParentInfo = &baseClassName::s_IDInfo;                                                               \
		if (s_IDInfo.id == INVALID_ID)                                                                                 \
		{                                                                                                              \
			return RESULT_OUT_OF_IDS;                                                                                  \
		}                                                                                                              \
		return RESULT_SUCCESS;                                                                                         \
	}                                                                                                                  \
	static Result UnregisterType()                                                                                     \
	{                                                                                                                  \
		NTT_ASSERT_RESULT_SUCCESS(IDSystem::FreeID(s_IDInfo.id));                                                      \
		s_IDInfo.pParentInfo = nullptr;                                                                                \
		return RESULT_SUCCESS;                                                                                         \
	}                                                                                                                  \
	NTT_OBJECT_TYPE_CHECK_METHOD(className)

#define NTT_OBJECT_DERIVED_DEFINE(className, baseClassName) IDInfo className::s_IDInfo = {INVALID_ID, nullptr};

/**
 * Base class for all objects inside the engine
 */
class Object
{
	NTT_OBJECT_BASE_DECLARE(Object)

public:
	Object();
	virtual ~Object();

public:
	void Print()
	{
		print("Object at %p\n", this);
	}
};

} // namespace ntt

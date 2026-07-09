#pragma once

#include "resource_type.h"
#include "services.h"

namespace ntt {

class Resource
{
public:
	Resource(ResourceType type);
	virtual ~Resource();

public:
	Result Load();
	Result Unload();

	inline bool IsLoaded() const
	{
		return m_Loaded;
	}

	inline bool IsLoading() const
	{
		return m_Loading;
	}

	inline bool IsUnloading() const
	{
		return m_Unloading;
	}

	inline bool HasError() const
	{
		return m_Error != RESULT_SUCCESS;
	}

	Result GetError() const
	{
		return m_Error;
	}

protected:
	virtual Result LoadImpl()	= 0;
	virtual Result UnloadImpl() = 0;

private:
	ResourceType m_type;
	bool		 m_Loaded;
	bool		 m_Loading;
	bool		 m_Unloading;
	Result		 m_Error;
};

} // namespace ntt

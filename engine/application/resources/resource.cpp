#include "resource.h"

namespace ntt {

Resource::Resource(ResourceType type)
	: m_type(type)
	, m_Loaded(false)
	, m_Loading(false)
	, m_Unloading(false)
	, m_Error(RESULT_SUCCESS)
{
}

Resource::~Resource()
{
	if (IsLoaded())
	{
		Unload();
	}
}

Result Resource::Load()
{
	if (m_Loaded)
	{
		return RESULT_RESOURCE_IS_ALREADY_LOADED;
	}

	if (m_Loading)
	{
		return RESULT_RESOURCE_IS_ALREADY_LOADING;
	}

	NTT_RESOURCE_DEBUG("Loading resource of type: %s", ToString(m_type));
	// TODO: Consider using multi-threading to load resources asynchronously in the future.
	m_Loading = true;
	m_Error	  = LoadImpl();
	m_Loading = false;

	if (m_Error == RESULT_SUCCESS)
	{
		NTT_RESOURCE_DEBUG("Resource of type %s loaded successfully.", ToString(m_type));
		m_Loaded = true;
	}
	else
	{
		NTT_RESOURCE_ERROR("Failed to load resource of type %s. Error: %s", ToString(m_type), ToString(m_Error));
	}

	return m_Error;
}

Result Resource::Unload()
{
	if (!m_Loaded)
	{
		return RESULT_RESOURCE_IS_ALREADY_LOADED;
	}

	if (m_Unloading)
	{
		return RESULT_RESOURCE_IS_ALREADY_UNLOADING;
	}

	NTT_RESOURCE_DEBUG("Unloading resource of type: %s", ToString(m_type));
	m_Unloading = true;
	m_Error		= UnloadImpl();
	m_Unloading = false;

	if (m_Error == RESULT_SUCCESS)
	{
		NTT_RESOURCE_DEBUG("Resource of type %s unloaded successfully.", ToString(m_type));
		m_Loaded = false;
	}
	else
	{
		NTT_RESOURCE_ERROR("Failed to unload resource of type %s. Error: %s", ToString(m_type), ToString(m_Error));
	}

	return m_Error;
}

} // namespace ntt

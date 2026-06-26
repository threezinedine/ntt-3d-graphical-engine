#pragma once

#include "core.h"
#include "object.h"

namespace ntt {

class RefCounted : public Object
{
public:
	RefCounted();
	virtual ~RefCounted();

	void AddRef();
	void Release();

private:
	int m_refCount;
};

} // namespace ntt

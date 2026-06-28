#include "ref_counted.h"

namespace ntt {

NTT_OBJECT_DERIVED_DEFINE(RefCounted, Object)

RefCounted::RefCounted()
	: m_refCount(0)
{
}

RefCounted::~RefCounted()
{
	NTT_ASSERT_MSG(m_refCount == 0, "RefCounted object destroyed with non-zero reference count.");
}

} // namespace ntt

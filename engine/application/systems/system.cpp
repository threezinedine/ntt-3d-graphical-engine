#include "system.h"

namespace ntt {

NTT_OBJECT_DERIVED_DEFINE(System, Object)

System::System()
{
}

System::~System()
{
}

Result System::Initialize()
{
	return InitializeImpl();
}

Result System::Shutdown()
{
	return ShutdownImpl();
}

} // namespace ntt
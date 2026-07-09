#pragma once

#include "object.h"
#include "ref_counted.h"

#include "application.h"
#include "systems/systems.h"

#include "resources/resources.h"

namespace ntt {

Result RegisterApplicationType();
Result UnregisterApplicationType();

} // namespace ntt

#pragma once

#include "core.h"
#include "object.h"
#include "services.h"

namespace ntt {

class Application : public Object
{
public:
	Application();
	Application(const Application&)		= delete;
	Application(Application&&) noexcept = delete;
	virtual ~Application();

	Application& operator=(const Application&)	   = delete;
	Application& operator=(Application&&) noexcept = delete;

public:
	Result Initialize(i32 argc, char** argv);
	Result Run();
	Result Shutdown();

protected:
	virtual Result InitializeImpl()
	{
		return RESULT_SUCCESS;
	}

	virtual Result ShutdownImpl()
	{
		return RESULT_SUCCESS;
	}

private:
};

} // namespace ntt
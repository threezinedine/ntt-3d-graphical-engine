#pragma once

#include "core.h"
#include "ecs.h"
#include "object.h"
#include "services.h"

namespace ntt {

typedef u32 RenderContextID;
typedef u32 WindowID;

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

	Result Update();
	Result UpdateWindow(WindowID windowID, RenderContextID renderContextID);

private:
	Scope<ECS> m_pEcs;
};

} // namespace ntt
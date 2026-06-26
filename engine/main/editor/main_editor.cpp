#include "applications.h"

using namespace ntt;

class EditorApplication : public Application
{
public:
};

int main(i32 argc, char** argv)
{
	EditorApplication app;

	NTT_ASSERT_RESULT_SUCCESS(app.Initialize(argc, argv));

	NTT_ASSERT_RESULT_SUCCESS(app.Run());

	NTT_ASSERT_RESULT_SUCCESS(app.Shutdown());

	return 0;
}
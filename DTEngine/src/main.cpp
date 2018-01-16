#include "Core/App.h"

#if DT_DEBUG
#include "vld.h"
#endif

#if DT_WINDOWS

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	{
		const UniquePtr<App>& app = App::GetInstance();
		if(app)
		{
			int exitCode = app->Run();
			App::FreeInstance();

			return exitCode;
		}
	}

	return -1;
}

#else

int main(int argc, char** argv)
{
	printf("Platform undefined, cannot start the app\n");
	system("pause");

	return 0;
}

#endif
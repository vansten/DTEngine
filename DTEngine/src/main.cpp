#include "Core/App.h"
#include "Core/Platform.h"

#include "GameFramework/GameObject.h"
#include "GameFramework/Components/Camera.h"

#include "Core/Window.h"

#if DT_WINDOWS

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	{
		App* app = {0};
		app = new App();
		if(app)
		{
			int exitCode = app->Run();
			delete app;
			app = nullptr;

#if DT_DEBUG
			if(_CrtDumpMemoryLeaks())
			{
				return APP_MEMORY_LEAKS;
			}
#endif

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
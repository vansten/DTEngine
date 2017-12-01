#include "Core/App.h"
#include "Core/Platform.h"

#include "GameFramework/GameObject.h"
#include "GameFramework/Components/Camera.h"

#include "Core/Window.h"
#include "Core/Event.h"

#if DT_DEBUG
#include "Includes/vld.h"
#endif

#if DT_WINDOWS

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	{
		const std::unique_ptr<App>& app = App::GetInstance();
		if(app)
		{
			int exitCode = app->Run();
			App::FreeInstance();

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
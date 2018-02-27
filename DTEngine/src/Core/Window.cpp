#include "Window.h"

#include "App.h"
#include "MessageSystem.h"
#include "Input.h"
#include "Debug/Debug.h"
#include "Rendering/Graphics.h"

Window gWindow;

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

bool Window::Open(const String& title, unsigned int width, unsigned int height)
{
	_title = title;
	_width = width;
	_height = height;
	_aspectRatio = (float)width / (float)height;

	// Window class registration
	WNDCLASSEX wndClassEx;

	wndClassEx.cbSize = sizeof(WNDCLASSEX);
	wndClassEx.style = CS_HREDRAW | CS_VREDRAW;
	wndClassEx.lpfnWndProc = WndProc;
	wndClassEx.cbClsExtra = 0;
	wndClassEx.cbWndExtra = 0;
	wndClassEx.hInstance = GetModuleHandle(NULL);
	wndClassEx.hIcon = (HICON)LoadIcon(wndClassEx.hInstance, IDI_APPLICATION);
	wndClassEx.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClassEx.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wndClassEx.lpszMenuName = NULL;
	wndClassEx.lpszClassName = DT_TEXT("DTEngineClass");
	wndClassEx.hIconSm = wndClassEx.hIcon;

	if (!RegisterClassEx(&wndClassEx))
	{
		gDebug.Print(LogVerbosity::Error, CHANNEL_ENGINE, DT_TEXT("Cannot register window class"));

		return false;
	}

	DWORD windowStyle = WS_OVERLAPPEDWINDOW;
	DWORD windowExStyle = WS_EX_CLIENTEDGE | WS_EX_WINDOWEDGE;

	RECT desiredClientRect;
	desiredClientRect.left = 0;
	desiredClientRect.right = _width;
	desiredClientRect.top = 0;
	desiredClientRect.bottom = _height;

	AdjustWindowRectEx(&desiredClientRect, windowStyle, false, windowExStyle);
	_width = desiredClientRect.right - desiredClientRect.left;
	_height = desiredClientRect.bottom - desiredClientRect.top;

	// Create window and store handle
	_hWnd = CreateWindowEx(
		windowExStyle,
		wndClassEx.lpszClassName,
		_title.c_str(),
		windowStyle,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		desiredClientRect.right - desiredClientRect.left,
		desiredClientRect.bottom - desiredClientRect.top,
		0,
		0,
		wndClassEx.hInstance,
		0
	);

	if (_hWnd == INVALID_HANDLE_VALUE)
	{
		gDebug.Print(LogVerbosity::Error, CHANNEL_ENGINE, DT_TEXT("Cannot create HWND"));

		return false;
	}

	return true;
}

bool Window::Show()
{
	ShowWindow(_hWnd, SW_SHOW);

	return IsWindowVisible(_hWnd);
}

bool Window::Hide()
{
	ShowWindow(_hWnd, SW_HIDE);

	return !IsWindowVisible(_hWnd);
}

bool Window::Close()
{
	Hide();
	DestroyWindow(_hWnd);

	return !IsWindow(_hWnd);
}

void Window::SetNewSize(unsigned int width, unsigned int height)
{
	if (_width == width && _height == height)
	{
		// Prevent resizing when current size already matches requested size
		return;
	}

	_width = width;
	_height = height;
	_aspectRatio = (float)_width / (float)_height;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Helper bool to catch only size msg in EXITSIZEMMOVE and ENTERSIZEMOVE
	static bool wasResizing = false;

	switch (msg)
	{
		case WM_ACTIVATE:
			{
				POINT point;
				GetCursorPos(&point);
				if (App::GetInstance()->IsRunning())
				{
					gInput.SetMousePosition(Vector2((float)point.x, (float)point.y));
				}
				break;
			}
		case WM_QUIT:
		case WM_DESTROY:
		case WM_CLOSE:
			{
				MessageSystem::PostQuit();
				break;
			}
		case WM_SIZE:
			{
				const unsigned int newWidth = LOWORD(lParam);
				const unsigned int newHeight = HIWORD(lParam);

				gWindow.SetNewSize(newWidth, newHeight);

				if (App::GetInstance()->IsRunning())
				{
					wasResizing = true;
					gGraphics.OnResize();
				}

				break;
			}
		case WM_ENTERSIZEMOVE:
			{
				gGraphics.BeginResize();
				break;
			}
		case WM_EXITSIZEMOVE:
			{
				if (wasResizing)
				{
					wasResizing = false;
					gGraphics.EndResize();
				}
				break;
			}
		case WM_KEYDOWN:
			{
				if (App::GetInstance()->IsRunning())
				{
					// 30 bit of lParam indicates whether key was down (1) or up (0) before message was sent
					// So in Down event I need to check if previous state of key was 'up' (not 'down')
					const bool wasUp = !(lParam & (1 << 30));
					if (wasUp)
					{
						gInput.OnKeyDown((int)wParam);
					}
				}
				break;
			}
		case WM_KEYUP:
			{
				if (App::GetInstance()->IsRunning())
				{
					// 30 bit of lParam indicates whether key was down (1) or up (0) before message was sent
					// So in Up event I need to check if previous state of key was 'down'
					const bool wasDown = lParam & (1 << 30);
					if (wasDown)
					{
						gInput.OnKeyUp((int)wParam);
					}
				}
				break;
			}
		case WM_MOUSEMOVE:
		case WM_NCMOUSEMOVE:
			{
				if (App::GetInstance()->IsRunning())
				{
					const int xPos = GET_X_LPARAM(lParam);
					const int yPos = GET_Y_LPARAM(lParam);

					gInput.SetMousePosition(Vector2((float)xPos, (float)yPos));
				}
				break;
			}
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:
			{
				if (App::GetInstance()->IsRunning())
				{
					// Down events can be handled in one case statement
					// Bitwise AND cause wParam can be something like MK_LBUTTON | MK_MBUTTON | ...
					if ((wParam & MK_LBUTTON) != 0)
					{
						gInput.OnMouseDown(VK_LBUTTON);
					}
					if ((wParam & MK_RBUTTON) != 0)
					{
						gInput.OnMouseDown(VK_RBUTTON);
					}
					if ((wParam & MK_MBUTTON) != 0)
					{
						gInput.OnMouseDown(VK_MBUTTON);
					}
				}

				SetCapture(hWnd);

				break;
			}
			// Up events cannot be (unfortunately) handled in one case statement as Down events...
		case WM_LBUTTONUP:
			{
				if (App::GetInstance()->IsRunning())
				{
					gInput.OnMouseUp(VK_LBUTTON);
				}

				ReleaseCapture();
				break;
			}
		case WM_RBUTTONUP:
			{
				if (App::GetInstance()->IsRunning())
				{
					gInput.OnMouseUp(VK_RBUTTON);
				}

				ReleaseCapture();
				break;
			}
		case WM_MBUTTONUP:
			{
				if (App::GetInstance()->IsRunning())
				{
					gInput.OnMouseUp(VK_MBUTTON);
				}

				ReleaseCapture();
				break;
			}
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}
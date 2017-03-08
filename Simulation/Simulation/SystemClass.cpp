#include "pch.h"
#include "SystemClass.h"



SystemClass::SystemClass():
	m_applicationName(nullptr),
	m_hinstance(nullptr),
	m_hwnd(nullptr),
	m_Config(nullptr)
{
}


SystemClass::~SystemClass()
{
	Shutdown();
}

bool SystemClass::Initialize()
{	
	bool result = false;

	m_Config = new ConfigClass;
	if (!m_Config)
	{
		MessageBoxA(NULL, "Unable to initialise the config class.", "Error", MB_OK | MB_ICONERROR);
		return false;
	}
	result = m_Config->Initialize();
	if (!result)
	{
		MessageBoxA(NULL, "Unable to initialise the config class.", "Error", MB_OK | MB_ICONERROR);
		return false;
	}
	
	if (!InitializeWindow(m_Config->GetScreenWidth(), m_Config->GetScreenHeight()))
	{
		MessageBoxA(NULL, "Unable to initialise the window.", "Error", MB_OK | MB_ICONERROR);
		return false;
	}

	return true;
}

void SystemClass::Shutdown()
{
	if (m_Config)
	{
		m_Config->Shutdown();
		delete m_Config;
		m_Config = 0;
	}
}

void SystemClass::Run()
{
	bool done, result;

	MSG msg;
	// Initialize the message structure.
	ZeroMemory(&msg, sizeof(MSG));

	// Loop until there is a quit message from the window or the user.
	done = false;
	while (!done)
	{
		// Handle the windows messages.
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		// If windows signals to end the application then exit out.
		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			CalculateFrameStats();
			
			result = Render();
			if (!result)
			{
				done = true;
			}
		}
	}
}

bool SystemClass::Render()
{
	return true;
}

bool SystemClass::InitializeWindow(int screenWidth, int screenHeight)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;


	// Get an external pointer to this object.	
	ApplicationHandle = this;

	// Get the instance of this application.
	m_hinstance = GetModuleHandle(nullptr);

	// Give the application a name.
	m_applicationName = L"08025ACW";


	// Setup the windows class with default settings.
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hinstance;
	wc.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = static_cast<HBRUSH> (GetStockObject(BLACK_BRUSH));
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = m_applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	// Register the window class.
	if (!RegisterClassEx(&wc))
	{
		MessageBoxA(NULL, "Unable to register the window class.", "Error", MB_OK | MB_ICONERROR);
	}

	// Determine the resolution of the clients desktop screen.
	//screenWidth = GetSystemMetrics(SM_CXSCREEN);
	//screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// Setup the screen settings depending on whether it is running in full screen or in windowed mode.
	if (m_Config->CheckFullScreen())
	{
		// If full screen set the screen to maximum size of the users desktop and 32bit.
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = static_cast<unsigned long> (screenWidth);
		dmScreenSettings.dmPelsHeight = static_cast<unsigned long> (screenHeight);
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Change the display settings to full screen.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// Set the position of the window to the top left corner.
		posX = posY = 0;
	}
	else
	{	
		// Place the window in the middle of the screen.
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	// Create the window with the screen settings and get the handle to it.
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, mMainWndCaption,
		WS_OVERLAPPEDWINDOW,
		posX, posY, screenWidth, screenHeight, nullptr, nullptr, m_hinstance, nullptr);

	//Check if window created
	if (!m_hwnd)
	{
		MessageBoxA(NULL, "Could not create the render window.", "Error", MB_OK | MB_ICONERROR);
		return false;
	}

	// Bring the window up on the screen and set it as main focus.
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	// Hide the mouse cursor.
	//ShowCursor(false);

	return true;
}

void SystemClass::ShutdownWindows()
{
	//ShowCursor(true);
	if (m_Config->CheckFullScreen())
	{
		ChangeDisplaySettings(nullptr, 0);
	}

	// Remove the window.
	DestroyWindow(m_hwnd);
	m_hwnd = nullptr;

	// Remove the application instance.
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = nullptr;

	// Release the pointer to this class.
	ApplicationHandle = nullptr;

	return;
}

void SystemClass::CalculateFrameStats()
{
}


LRESULT CALLBACK SystemClass::MessageHandler(const HWND  hwnd, const UINT umsg, const WPARAM wparam, const LPARAM lparam) const
{
	switch (umsg)
	{
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}

		// Check if the window is being closed.
		case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}

		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYUP:
			Keyboard::ProcessMessage(umsg, wparam, lparam);
			break;

		// Any other messages send to the default message handler as our application won't make use of them.
		default:
		{
			return DefWindowProc(hwnd, umsg, wparam, lparam);
		}
	}
}


LRESULT CALLBACK WndProc(const HWND hwnd, const UINT umessage, const WPARAM wparam, const LPARAM lparam)
{
	if (TwEventWin(hwnd, umessage, wparam, lparam))
		return 0;

	switch (umessage)
	{
		// Check if the window is being destroyed.
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}

		// Check if the window is being closed.
		case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}

		// All other messages pass to the message handler in the system class.
		default:
		{
			return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
		}
	}
}


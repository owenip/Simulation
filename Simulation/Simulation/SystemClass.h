#pragma once
#include "ConfigClass.h"
#include "GraphicClass.h"

class SystemClass
{
public:
	SystemClass();
	SystemClass(const SystemClass&) = default;
	SystemClass& operator=(const SystemClass&) = default;
	~SystemClass();

	bool Initialize();
	void Shutdown();
	void Run();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	bool Update();
	bool InitializeWindow(int screenWidth, int screenHeight);
	void ShutdownWindows();
	void CalculateFrameStats();

private:

	LPCWSTR m_applicationName;
	HINSTANCE m_hinstance;
	HWND m_hwnd;
	//std::wstring mMainWndCaption;

	
	//ConfigClass* mConfig;	
	shared_ptr<ConfigClass> mConfig;
	GraphicClass *mGraphic;
	TimerClass *mTimer;


	bool      mAppPaused;
};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
static SystemClass* ApplicationHandle = 0;
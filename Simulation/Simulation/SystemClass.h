#pragma once
#include "ConfigClass.h"
#include "GraphicClass.h"
#include "Simulation.h"
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
	shared_ptr<TimerClass> mTimer;
	shared_ptr<ConfigClass> mConfig;
	GraphicClass *mGraphic;	
	shared_ptr<Simulation> mSimulation;

	

	bool      mAppPaused;
};

const std::wstring mMainWndCaption = L"08025ACW";
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
static SystemClass* ApplicationHandle = 0;
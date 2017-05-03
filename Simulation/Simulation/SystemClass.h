#pragma once
#include "ConfigClass.h"
#include "GraphicClass.h"
#include "BallManagerClass.h"
#include "GravityWellManager.h"
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

	//Config
	std::shared_ptr<ConfigClass> mConfig;

	//Timer
	DX::StepTimer GraphicTimer;
	DX::StepTimer PhyTimer;
	DX::StepTimer NetworkTimer;

	//Graphic
	GraphicClass mGraphic;

	//Simulation
	Simulation mSimulation;

	//Ball Manager
	std::shared_ptr<BallManagerClass> mBallManger;

	//Gravity Manager
	std::shared_ptr<GravityWellManager> mGwManager;

	bool      mAppPaused;
};

const std::wstring mMainWndCaption = L"08024ACW";
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
static SystemClass* ApplicationHandle = 0;
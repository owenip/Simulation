#pragma once
#include "D3DClass.h"

class GraphicClass
{
public:
	GraphicClass();
	GraphicClass& operator=(const GraphicClass&) = default;
	~GraphicClass();

	bool Initialize(const HWND hwnd, const ConfigClass *mConfig);
	void Shutdown();

	void OnPause();
	void OnResume();

	bool Update();

	
private:
	bool InitAntTweak(const HWND hwnd);

	bool Render();

	void CheckInput();
private:
	ConfigClass *mConfig;
	D3DClass *mDirect3D;
	TwBar *mATBar;
	
	unique_ptr<Keyboard> m_keyboard;
	unique_ptr<Mouse> m_mouse;
	Keyboard::KeyboardStateTracker tracker;

	int mScreenWidth;
	int mScreenHeight;
	char mLastKeyPressed;

	int mNumberOfBalls = 9;
};


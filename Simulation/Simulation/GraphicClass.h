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

	bool Update();

	
private:
	bool InitAntTweak(const HWND hwnd);

	bool Render();

	void CheckInput();
private:
	ConfigClass *mConfig;
	D3DClass *mDirect3D;
	TwBar *mATBar;
	
	std::unique_ptr<DirectX::Keyboard> m_keyboard;
	std::unique_ptr<DirectX::Mouse> m_mouse;

	int mScreenWidth;
	int mScreenHeight;
	char mLastKeyPressed;
};


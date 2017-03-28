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
	bool Render();
	bool InitAntTweak(const HWND hwnd);

	

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

	//Ball
	int mNumberOfBalls = 9;
	std::unique_ptr<DirectX::GeometricPrimitive> mSphere;

	//DXTk 2D
	std::unique_ptr<DirectX::CommonStates> m_states;
	std::unique_ptr<DirectX::BasicEffect> m_effect;
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_batch;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;

};


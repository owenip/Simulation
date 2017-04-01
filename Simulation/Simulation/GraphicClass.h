#pragma once
#include "D3DClass.h"
#include "CameraClass.h"

class GraphicClass
{
public:
	GraphicClass();
	GraphicClass& operator=(const GraphicClass&) = default;
	~GraphicClass();

	bool Initialize(const HWND hwnd, const ConfigClass *mConfig, TimerClass *SysTimer);
	void Shutdown();

	void OnPause();
	void OnResume();

	bool Update();

	
private:
	bool Render();
	bool InitAntTweak(const HWND hwnd);	
	void CheckInput();

	//Gravity Well movement control
	void GwMove(SimpleMath::Vector3 direction);
	void GwMoveForward();
	void GwMoveBackward();
	void GwMoveLeft();
	void GwMoveRight();
private:
	ConfigClass *mConfig;
	D3DClass *mDirect3D;
	TwBar *mATBar;

	//Timer
	TimerClass *mTimer;
	
	unique_ptr<Keyboard> m_keyboard;
	unique_ptr<Mouse> m_mouse;
	int mouseX;
	int mouseY;
	Keyboard::KeyboardStateTracker tracker;

	int mScreenWidth;
	int mScreenHeight;
	char mLastKeyPressed;

	//Camera
	CameraClass *mCamera;

	//Ball
	int mNumberOfBalls = 9;

	//DXTk 2D
	std::unique_ptr<DirectX::CommonStates> m_states;
	std::unique_ptr<DirectX::BasicEffect> m_effect;
	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_batch;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;

	//DxTK 3D
	DirectX::SimpleMath::Matrix m_world;
	DirectX::SimpleMath::Matrix m_view;
	DirectX::SimpleMath::Matrix m_proj;

	//Gravity Well
	DirectX::SimpleMath::Vector3 mGravityWellPos;
	float mGWMovementGain;

	std::unique_ptr<DirectX::GeometricPrimitive> mSphere;

};


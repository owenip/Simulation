#pragma once
#include "pch.h"
#include "CameraClass.h"
#include "BallManagerClass.h"


class GraphicClass
{
public:
	GraphicClass();
	GraphicClass& operator=(const GraphicClass&) = default;
	~GraphicClass();

	bool Initialize(const HWND hwnd, shared_ptr<ConfigClass> Config, shared_ptr<TimerClass> SysTimer);
	void Shutdown();

	void OnPause();
	void OnResume();

	bool Update(float dt);
		
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
	void GwMoveByMouse(float &mouseX, float &mouseY);
	void GwMoveUp();
	void GwMoveDown();
private:
	shared_ptr<D3DClass> mDirect3D;
	shared_ptr<ConfigClass> mConfig;
	TwBar *mATBar;

	//Timer
	shared_ptr<TimerClass > mTimer;
	
	unique_ptr<Keyboard> m_keyboard;
	unique_ptr<Mouse> m_mouse;
	Keyboard::KeyboardStateTracker tracker;

	int mScreenWidth;
	int mScreenHeight;
	char mLastKeyPressed;

	//Camera
	unique_ptr<CameraClass> mCamera;

	//Ball
	int mNumberOfBalls;

	//DXTk 2D
	std::unique_ptr<DirectX::CommonStates> m_states;
	std::unique_ptr<DirectX::BasicEffect> m_effect;
	std::unique_ptr<DirectX::IEffect> mWallEffect;
	std::unique_ptr<DirectX::EffectFactory> m_fxFactory;

	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_batch;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;

	//DxTK 3D
	DirectX::SimpleMath::Matrix m_world;
	DirectX::SimpleMath::Matrix m_view;
	DirectX::SimpleMath::Matrix m_proj;

	//Models
	
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture;
	//Testing cup
	std::unique_ptr<DirectX::Model> mCup;
	std::unique_ptr<DirectX::EffectFactory> mCupfxFactory;

	//Gravity Well	
	DirectX::SimpleMath::Vector3 mGravityWellPos;
	float mGWMovementGain;
	std::unique_ptr<DirectX::GeometricPrimitive> mGravityWell;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> mGwInputLayout;

	//Surface
	std::unique_ptr<DirectX::Model> mSurface;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> mSurfaceInputLayout;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mSurfaceTex;

	//Wall
	//std::unique_ptr<DirectX::GeometricPrimitive> mWall;
	std::unique_ptr<DirectX::Model> mWall;
	std::unique_ptr<DirectX::EffectFactory> mWallfxFactory;

	//Balls
	std::unique_ptr<BallManagerClass> mBallManager;
};


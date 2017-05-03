#pragma once
#include "pch.h"
#include "CameraClass.h"
#include "BallManagerClass.h"
#include "GravityWellManager.h"

class GraphicClass
{
public:
	GraphicClass();
	GraphicClass& operator=(const GraphicClass&) = default;
	~GraphicClass();

	bool Initialize(const HWND hwnd, shared_ptr<ConfigClass> Config);
	void Shutdown();

	void OnPause();
	void OnResume();

	void Tick();
	bool Update(DX::StepTimer const& timer);

	//void SetSimulationPtr(shared_ptr<Simulation> InSimulation);
	void SetBallManagerPtr(shared_ptr<BallManagerClass> InBallManager);
	void SetGwManagerPtr(shared_ptr<GravityWellManager> InGwManager);

private:
	bool Render();
	bool InitAntTweak(const HWND hwnd);
	void CheckInput();
	

private:
	bool mIsEscaped;
	shared_ptr<D3DClass> mDirect3D;
	shared_ptr<ConfigClass> mConfig;
	TwBar *mATBar;

	//Changable Force
	float mFriction;
	float mElasticity;

	//Timer
	float timescale;
	DX::StepTimer mGraphicTimer;	
	double mActualFPS;

	//Target Freq
	float mTarGraphicFreq;
	float mTarPhysicsFreq;
	float mTarNetowrkFreq;

	//Input
	unique_ptr<Keyboard> m_keyboard;
	unique_ptr<Mouse> m_mouse;
	Keyboard::KeyboardStateTracker tracker;
	Mouse::ButtonStateTracker mousetracker;

	int mScreenWidth;
	int mScreenHeight;

	int mPeerID;

	//Ball
	int mNumberOfBalls;
	std::shared_ptr<BallManagerClass> mBallManager;

	//GravityWell
	std::shared_ptr<GravityWellManager> mGwManager;
	float mGwForce;
	//Camera
	std::unique_ptr<CameraClass> mCamera;	

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
	// Surface
	//std::unique_ptr<DirectX::Model> mSurface;
	std::unique_ptr<DirectX::GeometricPrimitive> mSurface;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> mSurfaceInputLayout;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mSurfaceTex;

	//Wall
	std::unique_ptr<DirectX::GeometricPrimitive> mWall;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> mWallInputLayout;
	//std::unique_ptr<DirectX::BasicEffect> mWallEffect;
	//std::unique_ptr<DirectX::Model> mWall;
	std::unique_ptr<DirectX::EffectFactory> mWallfxFactory;



};


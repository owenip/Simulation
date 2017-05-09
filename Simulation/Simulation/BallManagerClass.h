#pragma once
#include "pch.h"
#include "BallClass.h"

//using namespace DirectX;

class BallManagerClass
{
public:
	BallManagerClass();
	~BallManagerClass();

	bool Initialise(shared_ptr<ConfigClass> Config);
	bool Initialise(shared_ptr<D3DClass> Direct3D);

	void Update(float dt);
	void Render(SimpleMath::Matrix View);
	void Shutdown();

	void ClearAccumulator();
	void Integrate(float dt);

	void ReSetBallPosition();

	void GetBallIndex(std::vector<BallClass*>& BallIndex);
	std::vector<BallClass*> GetBallIndex();

	void GetSimIndex(std::vector<BallClass*>& SimIndex);
	std::vector<BallClass*> GetSimIndex();

	void ClearSimIndex();
	void AddSimBall(BallClass* InBall);

	void SetBallPos(int BallID, SimpleMath::Vector3 InPos);

	void SetBallRotatation(int BallID, SimpleMath::Vector3 InRotation);

private:
	void CreateBallIndex();
	void CreateTexture();


private:
	std::mutex mutex_BallManager;

	shared_ptr<D3DClass> mDirect3D;
	shared_ptr<ConfigClass> mConfig;

	int PeerID;
	int mNumberOfBalls;
	float mBallRadius;
	std::vector<BallClass*> mBallIndex;
	std::vector<BallClass*> mSimBallIndex;
	std::vector<BallClass*> mOwnedBallIndex;
	std::vector<BallClass*> mNetPendingBallIndex;

	std::unique_ptr<DirectX::GeometricPrimitive> mBallPrimitive;
	std::unique_ptr<DirectX::GeometricPrimitive> mMedBallPrimitive;
	std::unique_ptr<DirectX::GeometricPrimitive> mHeavyBallPrimitive;

	std::unique_ptr<DirectX::BasicEffect> m_Balleffect;
	std::unique_ptr<DirectX::BasicEffect> m_MedBalleffect;
	std::unique_ptr<DirectX::BasicEffect> m_HeavyBalleffect;

	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_MedinputLayout;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_HeavyinputLayout;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_LightTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_Mediumtexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_Heavytexture;

	
};


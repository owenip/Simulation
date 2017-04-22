#pragma once
#include "BallClass.h"
#include "ParticleClass.h"

class BallManagerClass:public BallClass
{
public:
	BallManagerClass();
	~BallManagerClass();

	bool Initialise(shared_ptr<D3DClass> Direct3D, shared_ptr<ConfigClass> Config);
	void Integrate(float dt);
	void Render(SimpleMath::Matrix View);
	void Shutdown();

	void GetBallIndex(std::vector<BallClass*> BallIndex);
	std::vector<BallClass*> GetBallIndex();

	void GetBallParticleIndex(std::vector<ParticleClass*> &Particles);
	std::vector<ParticleClass*> GetBallParticleIndex();

private:
	void CreateTexture();
	

private:
	shared_ptr<D3DClass> mDirect3D;
	shared_ptr<ConfigClass> mConfig;

	int mNumberOfBalls;
	float mBallRadius;
	std::vector<BallClass*> mBallIndex;

	std::unique_ptr<DirectX::GeometricPrimitive> mBallPrimitive;
	std::unique_ptr<DirectX::BasicEffect> m_Balleffect;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_LightTexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_Mediumtexture;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_Heavytexture;
};


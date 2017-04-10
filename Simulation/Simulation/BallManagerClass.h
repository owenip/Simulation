#pragma once
#include "BallClass.h"
#include "ParticleClass.h"

class BallManagerClass:public BallClass
{
public:
	BallManagerClass();
	~BallManagerClass();

	bool Initialise(D3DClass *Direct3D, ConfigClass *mConfig);
	void Update(float dt);
	void Render(SimpleMath::Matrix View);
	void Shutdown();

private:
	void CreateTexture();
	

private:
	D3DClass *mDirect3D;
	ID3D11Device *mDevice;
	ID3D11DeviceContext *mDeviceContext;
	ConfigClass *mConfig;

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


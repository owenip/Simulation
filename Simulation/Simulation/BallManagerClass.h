#pragma once
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

	void GetBallIndex(std::vector<BallClass*>& BallIndex) const;
	std::vector<BallClass*> GetBallIndex() const;



private:
	void CreateBallIndex();
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


#include "pch.h"
#include "BallManagerClass.h"
#include "BallClass.h"


BallManagerClass::BallManagerClass()
{
}


BallManagerClass::~BallManagerClass()
{
}

bool BallManagerClass::Initialise(D3DClass *Direct3D, ConfigClass *Config)
{
	assert(mDirect3D = Direct3D);
	assert(mDevice = mDirect3D->GetDevice());
	assert(mDeviceContext = mDirect3D->GetDeviceContext());
	assert(mConfig = Config);

	mNumberOfBalls = mConfig->GetNumberOfBalls();
	mBallRadius = mConfig->GetBallRadius();


	m_Balleffect = std::make_unique<DirectX::BasicEffect>(mDevice);
	m_Balleffect->SetTextureEnabled(true);
	CreateTexture();
	m_Balleffect->SetTexture(m_LightTexture.Get());


	mBallPrimitive = GeometricPrimitive::CreateSphere(mDeviceContext, mBallRadius * 2);
	mBallPrimitive->CreateInputLayout(m_Balleffect.get(),
		m_inputLayout.ReleaseAndGetAddressOf());
		
	for (int i = 0; i < mNumberOfBalls; i++)
	{
		BallClass *Ball = new BallClass();

		Ball->Initialize(-1, mBallRadius, 10.f, SimpleMath::Vector3(i * (mBallRadius * 2), mBallRadius, 0.f),
			SimpleMath::Vector3(0.f, 0.0f, 0.f), SimpleMath::Vector3(0.f, 0.f, 0.f), 0.99f);

		mBallIndex.push_back(Ball);
	}

	return true;
}

void BallManagerClass::Update(float dt)
{
	SimpleMath::Matrix Proj = SimpleMath::Matrix::Identity;
	mDirect3D->GetProj(Proj);
	m_Balleffect->SetProjection(Proj);

	for each (BallClass *Ball in mBallIndex)
	{
		Ball->Update(dt);
	}

	//May make balls sleep according to their accerlation and collision states
}

void BallManagerClass::Render(SimpleMath::Matrix View)
{
	SimpleMath::Matrix World = SimpleMath::Matrix::Identity;
	m_Balleffect->SetView(View);

	for each (BallClass *Ball in mBallIndex)
	{
		World = SimpleMath::Matrix::Identity;

		SimpleMath::Vector3 newPos;
		Ball->GetPosition(&newPos);

		World = World.CreateTranslation(newPos);
		m_Balleffect->SetWorld(World);

		mBallPrimitive->Draw(m_Balleffect.get(), m_inputLayout.Get());
	}

}

void BallManagerClass::Shutdown()
{	
	for (std::vector<BallClass*>::iterator iter = mBallIndex.begin(); iter!= mBallIndex.end(); ++iter)
	{
		delete *iter;
	}
	mBallIndex.clear();

	mBallPrimitive.reset();
	m_Balleffect.reset();
	m_inputLayout.Reset();
	m_LightTexture.Reset();
	m_Mediumtexture.Reset();
	m_Heavytexture.Reset();
}

void BallManagerClass::CreateTexture()
{
	DX::ThrowIfFailed(
		CreateWICTextureFromFile(mDevice, L".\\Resources\\earth.bmp", nullptr,
			m_LightTexture.ReleaseAndGetAddressOf()));
}

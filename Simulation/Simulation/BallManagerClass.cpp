#include "pch.h"
#include "BallManagerClass.h"
#include "BallClass.h"


BallManagerClass::BallManagerClass()
{
}


BallManagerClass::~BallManagerClass()
{
	this->Shutdown();
}

bool BallManagerClass::Initialise(shared_ptr<D3DClass> Direct3D, shared_ptr<ConfigClass> Config)
{
	assert(mDirect3D = Direct3D);
	assert(mConfig = Config);

	mNumberOfBalls = mConfig->GetNumberOfBalls();
	mBallRadius = mConfig->GetBallRadius();


	m_Balleffect = std::make_unique<DirectX::BasicEffect>(mDirect3D->GetDevice());
	m_Balleffect->SetTextureEnabled(true);
	CreateTexture();
	m_Balleffect->SetTexture(m_LightTexture.Get());


	mBallPrimitive = GeometricPrimitive::CreateSphere(mDirect3D->GetDeviceContext(), mBallRadius * 2);
	mBallPrimitive->CreateInputLayout(m_Balleffect.get(),
		m_inputLayout.ReleaseAndGetAddressOf());

	//Square generating pattern
	auto BallCenterDistance = mBallRadius * 2 + 0.5f;
	auto ProcessedBall = 0;
	auto CurrentDir = 1;
	auto CurSide = 1;
	auto CurSpawnX = 0.f;
	auto CurSpawnZ = 0.f;

	while (ProcessedBall <= mNumberOfBalls)
	{
		for (auto i = 0; i < 2; i++)
		{
			for (auto j = 0; j < CurSide; j++)
			{
				BallClass *Ball = new BallClass();
				SimpleMath::Vector3 SpawnPos(CurSpawnX, mBallRadius, CurSpawnZ);
				Ball->Initialize(i, -1, mBallRadius, 10.f, 
							SpawnPos,
							SimpleMath::Vector3::Zero, //Velocity
							SimpleMath::Vector3::Zero, //Accerlation
							0.99f);
				mBallIndex.push_back(Ball);
				ProcessedBall++;

				if (ProcessedBall > mNumberOfBalls)
				{
					break;
				}

				switch (CurrentDir)
				{
				case 0:
					CurSpawnZ += BallCenterDistance;
					break;
				case 1:
					CurSpawnX -= BallCenterDistance;
					break;
				case 2:
					CurSpawnZ -= BallCenterDistance;
					break;
				case 3:
					CurSpawnX += BallCenterDistance;
					break;
				}
			}
			if (ProcessedBall > mNumberOfBalls)
			{
				break;
			}
			CurrentDir++;
			if (CurrentDir >= 4)
			{
				CurrentDir = 0;
			}
		}
		if (ProcessedBall > mNumberOfBalls)
		{
			break;
		}
		CurSide++;

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

		if (Ball->GetPosition().y < mBallRadius)
		{
			Ball->SetPosition(Ball->GetPosition().x, mBallRadius, Ball->GetPosition().z);
			Ball->SetVelocity(SimpleMath::Vector3(0.f, 0.0f, 0.f));
		}
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

	mConfig.reset();
	mDirect3D.reset();
}

std::vector<BallClass*> BallManagerClass::GetBallIndex() const
{
	return mBallIndex;
}

void BallManagerClass::GetBallIndex(std::vector<BallClass*>& InIndex) const
{
	InIndex = mBallIndex;
}

void BallManagerClass::CreateTexture()
{
	DX::ThrowIfFailed(
		CreateWICTextureFromFile(mDirect3D->GetDevice(), L".\\Resources\\earth.bmp", nullptr,
			m_LightTexture.ReleaseAndGetAddressOf()));
}

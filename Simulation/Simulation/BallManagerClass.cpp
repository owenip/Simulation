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
		
	//for (int i = 0; i < mNumberOfBalls; i++)
	//{
	//	BallClass *Ball = new BallClass();

	//	Ball->Initialize(i,-1, mBallRadius, 10.f, SimpleMath::Vector3(i * (mBallRadius * 2), mBallRadius * 10.f, 0.f),
	//		SimpleMath::Vector3(0.f, -5.0f, 0.f), SimpleMath::Vector3(0.f, -2.f, 0.f), 0.99f);

	//	mBallIndex.push_back(Ball);
	//}

	//Square generating pattern
	auto BallCenterDistance = mBallRadius * 2 + 0.5f;
	auto ProcessedBall = 0;
	auto CurrentDir = 1;
	auto CurSide = 1;
	auto CurSpawnX = 0.f;
	auto CurSpawnZ = 0.f;

	while (ProcessedBall <= mNumberOfBalls)
	{
		//Direction 1:UP 2:Down 3:Left 4:Right
		

		for (auto i = 0; i < 2; i++)
		{
			for (auto j = 0; j < CurSide; j++)
			{
				BallClass *Ball = new BallClass();
				Vector3 SpawnPos(CurSpawnX, mBallRadius * 10.f, CurSpawnZ);
				Ball->Initialize(i, -1, mBallRadius, 10.f, 
							SpawnPos,
					SimpleMath::Vector3(-0.5f, -2.f, 0.f), //Velocity
					SimpleMath::Vector3(-0.5f, -2.f, 0.f), //Accerlation
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

	for each (BallClass *Ball in mBallIndex)
	for (mBallIndex:: i = mBallIndex.begin(); i < mBallIndex.end(); i++)
	{		
		Ball->SetVelocity(SimpleMath::Vector3(0.f, 0.0f, 0.f));
		Ball->SetAcceleration(SimpleMath::Vector3(0.f, 0.0f, 0.f));
	}

	
	//int Direction[4] = { 1,2,3,4 };

	////Process first three balls
	//SimpleMath::Vector3 StartPos = SimpleMath::Vector3(0.f, mBallRadius, 0.f);
	////1st ball
	//auto *Ball = new BallClass();
	//Ball->Initialize(0, -1, mBallRadius, 10.f, StartPos,
	//	SimpleMath::Vector3(0.f, -5.0f, 0.f), SimpleMath::Vector3(0.f, -2.f, 0.f), 0.99f);
	//mBallIndex.push_back(Ball);
	////2nd&3rd ball
	//for (auto i = 0; i < 1; i++)
	//{
	//	SimpleMath::Vector3 newPos(StartPos.x, StartPos.y, StartPos.z + BallCenterDistance);
	//	auto *Ball = new BallClass();
	//	newPos.x += BallCenterDistance * i;

	//	Ball->Initialize(0, -1, mBallRadius, 10.f, newPos,
	//		SimpleMath::Vector3(0.f, -5.0f, 0.f), SimpleMath::Vector3(0.f, -2.f, 0.f), 0.99f);
	//	mBallIndex.push_back(Ball);
	//}
	////Remaining Balls
	//for (auto i = 3; i < mNumberOfBalls; i++)
	//{
	//	auto *Ball = new BallClass();
	//	//Determine Direction then adjust startPos
	//	
	//	Ball->Initialize(0, -1, mBallRadius, 10.f, StartPos,
	//		SimpleMath::Vector3(0.f, -5.0f, 0.f), SimpleMath::Vector3(0.f, -2.f, 0.f), 0.99f);

	//	mBallIndex.push_back(Ball);
	//}

	//for (int i = 0; i < mNumberOfBalls; i++)
	//{
	//	BallClass *Ball = new BallClass();

	//	Ball->Initialize(i,-1, mBallRadius, 10.f, SimpleMath::Vector3(i * (mBallRadius * 2), mBallRadius * 10.f, 0.f),
	//		SimpleMath::Vector3(0.f, -5.0f, 0.f), SimpleMath::Vector3(0.f, -2.f, 0.f), 0.99f);

	//	mBallIndex.push_back(Ball);
	//}

	////Star pattarn
	//auto BallsPerLap = 8;
	//auto TotalLap = mNumberOfBalls / BallsPerLap;
	//auto LapInterval = mBallRadius * 4;
	//auto LeftBall = mNumberOfBalls % BallsPerLap;
	//auto CurrBallID = 0;
	//// Initialise first ball in the 1st lap
	//auto *newBall = new BallClass();
	//newBall->Initialize(CurrBallID, -1,
	//	mBallRadius, 10.f,
	//	SimpleMath::Vector3(0.f, mBallRadius, 0.f), SimpleMath::Vector3::Zero, SimpleMath::Vector3::Zero, 0.99f);
	//mBallIndex.push_back(newBall);
	//CurrBallID++;
	//for (auto i = 1; i <= TotalLap + 1; i++)
	//{
	//	Vector3 startPos(0.f, mBallRadius, LapInterval*i);
	//	SimpleMath::Quaternion qStartPos = Quaternion::Identity;
	//	if (!(i % 2))
	//	{
	//		//Rotate 45 degree
	//		//startPos = startPos.Transform(startPos, SimpleMath::Quaternion::CreateFromAxisAngle(Vector3::UnitY, 45.f));
	//		qStartPos = qStartPos.CreateFromAxisAngle(Vector3::UnitY, 90.f);
	//		startPos = startPos.Transform(startPos, qStartPos);
	//	}
	//	if (i == TotalLap + 1 && LeftBall > 0)
	//	{
	//		//Deal with 1-3 balls in last lap
	//		BallsPerLap = LeftBall;
	//	}
	//	// Initialise first ball in the current lap
	//	auto *newBall = new BallClass();
	//	newBall->Initialize(CurrBallID, -1,
	//		mBallRadius, 10.f,
	//		startPos, SimpleMath::Vector3::Zero, SimpleMath::Vector3::Zero, 0.99f);
	//	mBallIndex.push_back(newBall);
	//	CurrBallID++;
	//	// Initialise remaining balls in the current lap
	//	for (auto j = 1; j < BallsPerLap; j++)
	//	{
	//		//startPos = startPos.Transform(startPos, SimpleMath::Quaternion::CreateFromAxisAngle(Vector3::UnitY, 45.f));
	//		qStartPos = qStartPos.CreateFromAxisAngle(Vector3::UnitY, 45.f);
	//		startPos = startPos.Transform(startPos, qStartPos);
	//		auto *Ball = new BallClass();
	//		Ball->Initialize(CurrBallID, -1,
	//			mBallRadius, 10.f,
	//			startPos,
	//			SimpleMath::Vector3::Zero, SimpleMath::Vector3::Zero, 0.99f);
	//		mBallIndex.push_back(Ball);
	//		CurrBallID++;
	//	}
	//}

	return true;
}

void BallManagerClass::Update(float dt)
{
	SimpleMath::Matrix Proj = SimpleMath::Matrix::Identity;
	mDirect3D->GetProj(Proj);
	m_Balleffect->SetProjection(Proj);

	//for each (BallClass *Ball in mBallIndex)
	//{
	//	//Ball->Update(dt);

	//	if (Ball->GetPosition().y < mBallRadius)
	//	{
	//		Ball->SetPosition(Ball->GetPosition().x, mBallRadius, Ball->GetPosition().z);
	//		Ball->SetVelocity(SimpleMath::Vector3(0.f, 0.0f, 0.f));
	//		Ball->SetAcceleration(SimpleMath::Vector3(0.f, 0.0f, 0.f));
	//	}
	//}

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

std::vector<BallClass*> BallManagerClass::GetBallIndex()
{
	return mBallIndex;
}

void BallManagerClass::GetBallParticleIndex(std::vector<ParticleClass*>& Particles)
{
	Particles.clear();
	for (int i = 0; i < mNumberOfBalls; i++)
	{
		Particles.push_back(static_cast<ParticleClass*>(mBallIndex[i]));
	}
}

std::vector<ParticleClass*> BallManagerClass::GetBallParticleIndex()
{
	std::vector<ParticleClass*> Particles;
	for (int i = 0; i < mNumberOfBalls; i++)
	{
		Particles.push_back(static_cast<ParticleClass*>(mBallIndex[i]));
	}
	return Particles;
}


void BallManagerClass::CreateTexture()
{
	DX::ThrowIfFailed(
		CreateWICTextureFromFile(mDirect3D->GetDevice(), L".\\Resources\\earth.bmp", nullptr,
			m_LightTexture.ReleaseAndGetAddressOf()));
}

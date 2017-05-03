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


bool BallManagerClass::Initialise(shared_ptr<ConfigClass> Config)
{
	assert(mConfig = Config);
	mNumberOfBalls = mConfig->GetNumberOfBalls();
	mBallRadius = mConfig->GetBallRadius();
	
	this->CreateBallIndex();

	return false;
}

bool BallManagerClass::Initialise(shared_ptr<D3DClass> Direct3D)
{
	assert(mDirect3D = Direct3D);

	m_Balleffect = std::make_unique<DirectX::BasicEffect>(mDirect3D->GetDevice());
	m_Balleffect->SetTextureEnabled(true);
	CreateTexture();
	m_Balleffect->SetTexture(m_LightTexture.Get());

	mBallPrimitive = GeometricPrimitive::CreateSphere(mDirect3D->GetDeviceContext(), mBallRadius * 2);
	mBallPrimitive->CreateInputLayout(m_Balleffect.get(),
		m_inputLayout.ReleaseAndGetAddressOf());



	return true;
}

void BallManagerClass::ClearAccumulator()
{
	for each (BallClass *Ball in mBallIndex) 
	{
		Ball->ClearAccumulator();
	}
}

void BallManagerClass::Integrate(float dt)
{
	for each (BallClass *Ball in mBallIndex)
	{
		Ball->Integrate(dt);	
	}
}

void BallManagerClass::Update(float dt)
{
	for each (auto Ball in mBallIndex)
	{
		SimpleMath::Vector3 newPos, newVelocity , newtotation;
		Ball->GetPosition(newPos);
		Ball->GetVelocity(newVelocity);

		//Create yaw pitch row
		newtotation.y = atan2f(Ball->mLastPosition.x - newPos.x, Ball->mLastPosition.z - newPos.z);
		newtotation.x -= abs(newVelocity.x * dt) + abs(newVelocity.z * dt);
		//newtotation.z = 0.f;

		Ball->SetRotation(newtotation);
	}
}

void BallManagerClass::Render(SimpleMath::Matrix View)
{
	SimpleMath::Matrix Proj = SimpleMath::Matrix::Identity;
	mDirect3D->GetProj(Proj);
	m_Balleffect->SetProjection(Proj);
	m_Balleffect->SetView(View);
		
	for each (auto Ball in mBallIndex)
	{
		SimpleMath::Vector3 newPos,newVelocity;
		Ball->GetPosition(newPos);
		Ball->GetVelocity(newVelocity);	

		SimpleMath::Matrix  World = SimpleMath::Matrix::CreateFromYawPitchRoll(Ball->GetRotation().y, Ball->GetRotation().x, 0.f);

		World.Translation(newPos);
		m_Balleffect->SetWorld(World);

		mBallPrimitive->Draw(m_Balleffect.get(), m_inputLayout.Get());
	}

}

void BallManagerClass::Shutdown()
{	
	for (vector<BallClass*>::iterator iter = mBallIndex.begin(); iter!= mBallIndex.end(); ++iter)
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

void BallManagerClass::GetBallIndex(std::vector<BallClass*> &BallIndex) const
{
	BallIndex = mBallIndex;
}

std::vector<BallClass*> BallManagerClass::GetBallIndex() const
{
	return mBallIndex;
}


void BallManagerClass::CreateBallIndex()
{
	mBallIndex.clear();
	//Spawn along x axis
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
				SimpleMath::Vector3 SpawnPos(CurSpawnX, mBallRadius * 10.f, CurSpawnZ);
				Ball->Initialize(ProcessedBall, -1, mBallRadius, 10.f,
					SpawnPos,
					SimpleMath::Vector3(05.f, 0.f, 1.f), //Velocity
					SimpleMath::Vector3(0.f, 0.f, -0.1f), //Accerlation
					0.90f);
				mBallIndex.push_back(Ball);
				ProcessedBall++;

				if (ProcessedBall >= mNumberOfBalls)
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
			if (ProcessedBall >= mNumberOfBalls)
			{
				break;
			}
			CurrentDir++;
			if (CurrentDir >= 4)
			{
				CurrentDir = 0;
			}
		}
		if (ProcessedBall >= mNumberOfBalls)
		{
			break;
		}
		CurSide++;

	}

	for (auto i = 0; i < mBallIndex.size(); i++)
	{
		if (i % 2)
		{
			mBallIndex[i]->SetVelocity(SimpleMath::Vector3(6.65f * i, -4.f, 6.3f * i));
			mBallIndex[i]->SetAcceleration(SimpleMath::Vector3(0.f,0.f,0.f));
		}
	}
}

void BallManagerClass::CreateTexture()
{
	DX::ThrowIfFailed(
		CreateWICTextureFromFile(mDirect3D->GetDevice(), L".\\Resources\\earth.bmp", nullptr,
			m_LightTexture.ReleaseAndGetAddressOf()));
}

#include "pch.h"
#include "BallManagerClass.h"
#include "BallClass.h"


BallManagerClass::BallManagerClass()
{
}


BallManagerClass::~BallManagerClass()
{
	//mDirect3D.reset();
	//mConfig.reset();
}

bool BallManagerClass::Initialise(shared_ptr<ConfigClass> Config)
{
	assert(mConfig = Config);
	mNumberOfBalls = mConfig->GetNumberOfBalls();
	mBallRadius = mConfig->GetBallRadius();
	PeerID = mConfig->GetPeerID();

	mSimBallIndex.reserve(mNumberOfBalls);
	mOwnedBallIndex.reserve(mNumberOfBalls);
	mBallIndex.reserve(mNumberOfBalls);

	this->CreateBallIndex();

	return true;
}

bool BallManagerClass::Initialise(shared_ptr<D3DClass> Direct3D)
{
	assert(mDirect3D = Direct3D);
	CreateTexture();
	//Light
	m_Balleffect = std::make_unique<DirectX::BasicEffect>(mDirect3D->GetDevice());
	m_Balleffect->SetPerPixelLighting(true);
	m_Balleffect->SetLightingEnabled(true);
	m_Balleffect->SetAmbientLightColor(Colors::White);
	m_Balleffect->SetLightEnabled(0, true);
	m_Balleffect->SetTextureEnabled(true);	
	m_Balleffect->SetTexture(m_LightTexture.Get());
	mBallPrimitive = GeometricPrimitive::CreateSphere(mDirect3D->GetDeviceContext(), mBallRadius * 2);
	mBallPrimitive->CreateInputLayout(m_Balleffect.get(),
		m_inputLayout.ReleaseAndGetAddressOf());

	//Medium
	m_MedBalleffect = std::make_unique<DirectX::BasicEffect>(mDirect3D->GetDevice());
	m_MedBalleffect->SetPerPixelLighting(true);
	m_MedBalleffect->SetLightingEnabled(true);
	m_MedBalleffect->SetAmbientLightColor(Colors::White);
	m_MedBalleffect->SetLightEnabled(0, true);
	m_MedBalleffect->SetTextureEnabled(true);
	m_MedBalleffect->SetTexture(m_Mediumtexture.Get());
	mMedBallPrimitive = GeometricPrimitive::CreateSphere(mDirect3D->GetDeviceContext(), mBallRadius * 2);
	mMedBallPrimitive->CreateInputLayout(m_MedBalleffect.get(),
		m_MedinputLayout.ReleaseAndGetAddressOf());

	//Heavy	
	m_HeavyBalleffect = std::make_unique<DirectX::BasicEffect>(mDirect3D->GetDevice());
	m_HeavyBalleffect->SetPerPixelLighting(true);
	m_HeavyBalleffect->SetLightingEnabled(true);
	m_HeavyBalleffect->SetAmbientLightColor(Colors::White);
	m_HeavyBalleffect->SetLightEnabled(0, true);
	m_HeavyBalleffect->SetTextureEnabled(true);
	m_HeavyBalleffect->SetTexture(m_Heavytexture.Get());
	mHeavyBallPrimitive = GeometricPrimitive::CreateSphere(mDirect3D->GetDeviceContext(), mBallRadius * 2);
	mHeavyBallPrimitive->CreateInputLayout(m_HeavyBalleffect.get(),
		m_HeavyinputLayout.ReleaseAndGetAddressOf());

	return true;
}

void BallManagerClass::ClearAccumulator()
{
	if (mConfig->GetDisplayAll() == false)
	{
		for (BallClass *Ball : mSimBallIndex)
		{
			Ball->ClearAccumulator();
		}
	}
	else
	{
		for (BallClass *Ball : mBallIndex)
		{
			Ball->ClearAccumulator();
		}
	}
}

void BallManagerClass::Integrate(float dt)
{
	if (mConfig->GetDisplayAll() == false)
	{
		for (BallClass *Ball : mSimBallIndex)
		{
			if (Ball->GetOwenerID() != mConfig->GetPeerID())
				continue;
			Ball->Integrate(dt);
		}
	}
	else
	{
		for (BallClass *Ball : mBallIndex)
		{
			if (Ball->GetOwenerID() != mConfig->GetPeerID())
				continue;
			Ball->Integrate(dt);
		}
	}
}

void BallManagerClass::ReSetBallPosition()
{
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

				SimpleMath::Vector3 SpawnPos(CurSpawnX, mBallRadius * 10.f, CurSpawnZ);
				SimpleMath::Vector3 Zero(SimpleMath::Vector3::Zero);
				mBallIndex[ProcessedBall]->SetPosition(SpawnPos);
				mBallIndex[ProcessedBall]->SetRotation(Zero);
				mBallIndex[ProcessedBall]->SetVelocity(Zero);
				mBallIndex[ProcessedBall]->SetAcceleration(Zero);
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
}

void BallManagerClass::Update(float dt)
{
	if (mConfig->GetDisplayAll() == false)
	{
		for (auto Ball : mSimBallIndex)
		{
			SimpleMath::Vector3 newPos, newVelocity, newtotation;
			Ball->GetPosition(newPos);
			Ball->GetVelocity(newVelocity);
			Ball->GetRotation(newtotation);
			//Create yaw pitch row
			newtotation.y = atan2f(Ball->mLastPosition.x - newPos.x, Ball->mLastPosition.z - newPos.z);
			newtotation.x -= abs(newVelocity.x* dt) + abs(newVelocity.z * dt);

			if (newtotation.x >= XM_PI * 2)
				newtotation.x = 0;
			newtotation.z = 0.f;

			Ball->SetRotation(newtotation);
		}
	}
	else
	{
		for (auto Ball : mBallIndex)
		{
			SimpleMath::Vector3 newPos, newVelocity, newtotation;
			Ball->GetPosition(newPos);
			Ball->GetVelocity(newVelocity);
			Ball->GetRotation(newtotation);
			//Create yaw pitch row
			newtotation.y = atan2f(Ball->mLastPosition.x - newPos.x, Ball->mLastPosition.z - newPos.z);
			newtotation.x -= abs(newVelocity.x* dt) + abs(newVelocity.z * dt);

			if (newtotation.x >= XM_PI * 2)
				newtotation.x = 0;
			newtotation.z = 0.f;

			Ball->SetRotation(newtotation);
		}
	}
}

void BallManagerClass::Render(SimpleMath::Matrix View)
{
	std::lock_guard<std::mutex> BallManager_guard(mutex_BallManager);
	SimpleMath::Matrix Proj = SimpleMath::Matrix::Identity;
	mDirect3D->GetProj(Proj);
	m_Balleffect->SetProjection(Proj);
	m_Balleffect->SetView(View);
	m_Balleffect->SetLightDirection(0, -SimpleMath::Vector3::UnitY);
	m_Balleffect->SetSpecularPower(1.f);

	if (mConfig->GetDisplayAll() == false)
	{
		if (mSimBallIndex.size() > 0)
		{
			for (auto Ball : mSimBallIndex)
			{
				if (Ball->GetOwenerID() == PeerID)
				{
					SimpleMath::Color c(0, 0, 0, 0.5f);
					if (Ball->GetOwenerID() == 0)
					{
						m_Balleffect->SetLightSpecularColor(0, Colors::Wheat);
					}
					else if (Ball->GetOwenerID() == 1)
					{
						m_Balleffect->SetLightDiffuseColor(0, Colors::Green);
					}
					else if (Ball->GetOwenerID() == 2)
					{
						m_Balleffect->SetLightDiffuseColor(0, Colors::Blue);
					}

					if (Ball->GetMass() == 1.f)
					{
						m_Balleffect->SetTexture(m_LightTexture.Get());
					}
					else if (Ball->GetMass() == 2.f)
					{
						m_Balleffect->SetTexture(m_Mediumtexture.Get());
					}
					else if (Ball->GetMass() == 5.f)
					{
						m_Balleffect->SetTexture(m_Heavytexture.Get());
					}

					SimpleMath::Vector3 newPos;
					Ball->GetPosition(newPos);
					SimpleMath::Matrix  World = SimpleMath::Matrix::CreateFromYawPitchRoll(Ball->GetRotation().y, Ball->GetRotation().x, 0.f);

					World.Translation(newPos);
					m_Balleffect->SetWorld(World);
					mBallPrimitive->Draw(m_Balleffect.get(), m_inputLayout.Get());

				}
			}
		}
	}
	else
	{
		for (auto Ball : mBallIndex)
		{			
			//if (Ball->GetOwenerID() == PeerID)
			{
				SimpleMath::Color c(0, 0, 0, 0.5f);
				if (Ball->GetOwenerID() == 0)
				{
					m_Balleffect->SetLightSpecularColor(0, Colors::Wheat);
				}
				else if (Ball->GetOwenerID() == 1)
				{
					m_Balleffect->SetLightDiffuseColor(0, Colors::Green);
				}
				else if (Ball->GetOwenerID() == 2)
				{
					m_Balleffect->SetLightDiffuseColor(0, Colors::Blue);
				}

				if (Ball->GetMass() == 1.f)
				{
					m_Balleffect->SetTexture(m_LightTexture.Get());
				}
				else if (Ball->GetMass() == 2.f)
				{
					m_Balleffect->SetTexture(m_Mediumtexture.Get());
				}
				else if (Ball->GetMass() == 5.f)
				{
					m_Balleffect->SetTexture(m_Heavytexture.Get());
				}

				SimpleMath::Vector3 newPos;
				Ball->GetPosition(newPos);
				SimpleMath::Matrix  World = SimpleMath::Matrix::CreateFromYawPitchRoll(Ball->GetRotation().y, Ball->GetRotation().x, 0.f);

				World.Translation(newPos);
				m_Balleffect->SetWorld(World);
				mBallPrimitive->Draw(m_Balleffect.get(), m_inputLayout.Get());
			}
		}
	}
}

void BallManagerClass::Shutdown()
{	
	mBallIndex.clear();

	mBallPrimitive.reset();
	mMedBallPrimitive.reset();
	mHeavyBallPrimitive.reset();

	m_Balleffect.reset();
	m_MedBalleffect.reset();
	m_HeavyBalleffect.reset();

	m_inputLayout.Reset();
	m_MedinputLayout.Reset();
	m_HeavyinputLayout.Reset();

	m_LightTexture.Reset();
	m_Mediumtexture.Reset();
	m_Heavytexture.Reset();

	mConfig.reset();
	mDirect3D.reset();
}

void BallManagerClass::GetBallIndex(std::vector<BallClass*> &BallIndex)
{
	BallIndex = mBallIndex;
}

std::vector<BallClass*> BallManagerClass::GetBallIndex()
{	
	return mBallIndex;
}

void BallManagerClass::GetSimIndex(std::vector<BallClass*>& SimIndex)
{
	mSimBallIndex = SimIndex;
}

std::vector<BallClass*> BallManagerClass::GetSimIndex()
{
	return mSimBallIndex;
}

void BallManagerClass::ClearSimIndex()
{
	std::lock_guard<std::mutex> BallManager_guard(mutex_BallManager);
	mSimBallIndex.clear();
}

void BallManagerClass::AddSimBall(BallClass * InBall)
{
	std::lock_guard<std::mutex> BallManager_guard(mutex_BallManager);
	mSimBallIndex.push_back(InBall);
}

void BallManagerClass::SetBallPos(int BallID, SimpleMath::Vector3 InPos)
{
	//std::lock_guard<std::mutex> BallManager_guard(mutex_BallManager);	
	mBallIndex[BallID]->SetPosition(InPos);	
}

void BallManagerClass::SetBallRotatation(int BallID, SimpleMath::Vector3 InRotation)
{
	std::lock_guard<std::mutex> BallManager_guard(mutex_BallManager);	
	mBallIndex[BallID]->SetRotation(InRotation);	
}

void BallManagerClass::CreateBallIndex()
{
	//std::lock_guard<std::mutex> BallManager_guard(mutex_BallManager);
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
				
				SimpleMath::Vector3 SpawnPos(CurSpawnX, mBallRadius, CurSpawnZ);
				BallClass *Ball = new BallClass(ProcessedBall, 0, mBallRadius, 1.f,
					SpawnPos);
				//Ball->Initialize(ProcessedBall, PeerID, mBallRadius, 10.f,
				//	SpawnPos,
				//	SimpleMath::Vector3(05.f, 0.f, 1.f), //Velocity
				//	SimpleMath::Vector3(0.f, 0.f, -0.1f), //Accerlation
				//	0.90f);
				//Ball->AddForce(SimpleMath::Vector3(-010.f * i, -9.81f, 15.f));
				
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

	for (int i = 0; i < mBallIndex.size() - 3; i+=3)
	{
		if (i % 2)
		{
			//mBallIndex[i]->AddForce(SimpleMath::Vector3(05.f, -0.981f, 50.f));
			/*mBallIndex[i]->SetVelocity(SimpleMath::Vector3(6.65f * i, -4.f, 6.3f * i));
			mBallIndex[i]->SetAcceleration(SimpleMath::Vector3(0.f, 0.f, 0.f));*/
			mBallIndex[i]->SetOwenerID(1);
			//mBallIndex[i]->SetMass(150.f);
		}
		mBallIndex[i + 1]->SetMass(2.f);
		mBallIndex[i + 2]->SetMass(5.f);
	}
}

void BallManagerClass::CreateTexture()
{
	DX::ThrowIfFailed(
		CreateWICTextureFromFile(mDirect3D->GetDevice(), L".\\Resources\\tennis.png", nullptr,
			m_LightTexture.ReleaseAndGetAddressOf()));
	DX::ThrowIfFailed(
		CreateWICTextureFromFile(mDirect3D->GetDevice(), L".\\Resources\\basketball.jpg", nullptr,
			m_Mediumtexture.ReleaseAndGetAddressOf()));
	DX::ThrowIfFailed(
		CreateWICTextureFromFile(mDirect3D->GetDevice(), L".\\Resources\\earth.bmp", nullptr,
			m_Heavytexture.ReleaseAndGetAddressOf()));
}


#include "pch.h"
#include "GravityWellManager.h"



GravityWellManager::GravityWellManager() :
	mGwRadius(0.f),
	mLocalPeerID(0),
	mGWMovementGain(0.0005f),
	mGwForceGain(0.0005f)
{
}


GravityWellManager::~GravityWellManager()
{
}


bool GravityWellManager::Initialise(shared_ptr<ConfigClass> Config)
{
	mConfig = Config;
	mGwRadius = mConfig->GetGwRadius();
	mLocalPeerID = mConfig->GetPeerID();
	mGwIndex.clear();
	/*SimpleMath::Color GwColor;	
	switch (mLocalPeerID)
	{
		case 0:
		{
			GwColor = Colors::Red;
			break;
		}
		case 1:
		{
			GwColor = Colors::Green;
			break;
		}
		case 2:
		{
			GwColor = Colors::Blue;
			break;
		}
	}*/
	this->AddGw(0, SimpleMath::Vector3::Zero, static_cast<SimpleMath::Color>(Colors::Red));
	this->AddGw(1, SimpleMath::Vector3::Zero, static_cast<SimpleMath::Color>(Colors::Green));
	this->AddGw(2, SimpleMath::Vector3::Zero, static_cast<SimpleMath::Color>(Colors::Blue));

	mGwIndex[mLocalPeerID]->SetIsActive(true);
	
	return true;
}

bool GravityWellManager::InitialiseGraphic(shared_ptr<D3DClass> InDirect3D)
{
	assert(mDirect3D = InDirect3D);
	mStates = std::make_unique<DirectX::CommonStates>(mDirect3D->GetDevice());

	mGwEffect = make_unique<DirectX::BasicEffect>(mDirect3D->GetDevice());
	//mGwEffect->SetAlpha(.3f);

	mGwPrimitive = GeometricPrimitive::CreateSphere(mDirect3D->GetDeviceContext(), mGwRadius * 2);
	mGwPrimitive->CreateInputLayout(mGwEffect.get(), mGwInputLayout.ReleaseAndGetAddressOf());

	//Center Indication for Gw
	mGwCenter = GeometricPrimitive::CreateSphere(mDirect3D->GetDeviceContext(), mGwRadius / 10.f);

	return true;
}

void GravityWellManager::Render(SimpleMath::Matrix InView)
{	
	mLocalPeerID = mConfig->GetPeerID();
	SimpleMath::Matrix Proj = SimpleMath::Matrix::Identity;
	mDirect3D->GetProj(Proj);
	mGwEffect->SetProjection(Proj);
	mGwEffect->SetView(InView);
	std::lock_guard<std::mutex> pt_guard(mustex_gw);
	for (GravityWellClass* element : mGwIndex)
	{
		if (element->GetIsActive())
		{
			SimpleMath::Matrix World = SimpleMath::Matrix::Identity;
			SimpleMath::Vector3 newPos = element->GetPos();
			World = World.CreateTranslation(newPos);
			mGwEffect->SetWorld(World);

			//Settup color for this Gw
			SimpleMath::Color GwColor = element->GetColor();
			//mGwEffect->SetColorAndAlpha(GwColor);
			//Draw Center of Gw
			mGwCenter->Draw(World, InView, Proj, GwColor);
			//Change to Transparent effect
			GwColor.w = 0.3f;
			mGwEffect->SetColorAndAlpha(GwColor);
			//Draw the Gw
			mGwPrimitive->Draw(mGwEffect.get(), mGwInputLayout.Get(), true, false, [=]
			{
				mDirect3D->GetDeviceContext()->RSSetState(mStates->CullNone());
			});
		}
	}
}

void GravityWellManager::Shutdown()
{
	mDirect3D.reset();

	mStates.reset();
	mGwPrimitive.reset();
	mGwCenter.reset();
	mGwEffect.reset();
	mGwInputLayout.Reset();

	for (vector<GravityWellClass*>::iterator iter = mGwIndex.begin(); iter != mGwIndex.end(); ++iter)
	{
		delete *iter;
	}
	mGwIndex.clear();
}

void GravityWellManager::SetGwRadius(float InGwRadius)
{
	mGwRadius = InGwRadius;
}

float GravityWellManager::GetGwRadius() const
{
	return mGwRadius;
}


void GravityWellManager::AddGw(int GwID)
{
	std::lock_guard<std::mutex> pt_guard(mustex_gw);
	SimpleMath::Color GwColor;
	switch (GwID)
	{
		case 0:
		{
			GwColor = Colors::Red;
			break;
		}
		case 1:
		{
			GwColor = Colors::Green;
			break;
		}
		case 2:
		{
			GwColor = Colors::Blue;
			break;
		}
	}
	this->AddGw(GwID, SimpleMath::Vector3::Zero, GwColor);
}

void GravityWellManager::AddGw(int InGwID, SimpleMath::Vector3 InGravityWellPos, SimpleMath::Color InGwColor)
{
	std::lock_guard<std::mutex> pt_guard(mustex_gw);
	GravityWellClass *newGw = new GravityWellClass();
	newGw->Initialize(InGwID, InGravityWellPos, InGwColor);
	mGwIndex.push_back(newGw);
}

void GravityWellManager::RemoveGw(int GwID)
{
	std::lock_guard<std::mutex> pt_guard(mustex_gw);
	for (vector<GravityWellClass*>::iterator iter = mGwIndex.begin();
		iter != mGwIndex.end(); ++iter)
	{
		if ((*iter)->GetGwID() == GwID)
		{
			mGwIndex.erase(iter);
			break;
		}
	}
}

void GravityWellManager::SetGwIsActive(int GwID, bool activeState)
{
	std::lock_guard<std::mutex> pt_guard(mustex_gw);
	mGwIndex[GwID]->SetIsActive(activeState);
}

void GravityWellManager::SetLocalID(int GwID)
{
	mLocalPeerID = GwID;
}

void GravityWellManager::GwSetPos(int GwID, SimpleMath::Vector3 InGravityWellPos)
{
	std::lock_guard<std::mutex> pt_guard(mustex_gw);
	for (auto Gw: mGwIndex)
	{
		if (Gw->GetGwID() == GwID)
		{
			Gw->SetPos(InGravityWellPos);
			break;
		}
	}
}

SimpleMath::Vector3 GravityWellManager::GwGetPos(int GwID)
{
	std::lock_guard<std::mutex> pt_guard(mustex_gw);
	for (vector<GravityWellClass*>::iterator iter = mGwIndex.begin();
		iter != mGwIndex.end(); ++iter)
	{
		if ((*iter)->GetGwID() == GwID)
		{
			return (*iter)->GetPos();
		}		
	}
}


void GravityWellManager::GwAddMove(SimpleMath::Vector3 InMove)
{
	mLocalPeerID = mConfig->GetPeerID();
	std::lock_guard<std::mutex> pt_guard(mustex_gw);
	for (vector<GravityWellClass*>::iterator iter = mGwIndex.begin();
		iter != mGwIndex.end(); ++iter)
	{
		if ((*iter)->GetGwID() == mLocalPeerID)
		{
			(*iter)->AddMove(InMove * mGWMovementGain);
			break;
		}
	}
}

void GravityWellManager::GwMoveForward()
{
	this->GwAddMove(SimpleMath::Vector3::Forward);
}

void GravityWellManager::GwMoveBackward()
{
	this->GwAddMove(SimpleMath::Vector3::Backward);
}

void GravityWellManager::GwMoveLeft()
{
	this->GwAddMove(SimpleMath::Vector3::Left);
}

void GravityWellManager::GwMoveRight()
{
	this->GwAddMove(SimpleMath::Vector3::Right);
}

void GravityWellManager::GwMoveByMouse(float mouseX, float mouseY)
{
	SimpleMath::Vector3 move = SimpleMath::Vector3(mouseX * 100, 0.f, mouseY * 100);
	this->GwAddMove(move);
}

void GravityWellManager::GwMoveUp()
{
	this->GwAddMove(SimpleMath::Vector3::Up);
}

void GravityWellManager::GwMoveDown()
{
	this->GwAddMove(SimpleMath::Vector3::Down);
}

void GravityWellManager::GwAddAttractF()
{
	std::lock_guard<std::mutex> pt_guard(mustex_gw);
	for (vector<GravityWellClass*>::iterator iter = mGwIndex.begin();
		iter != mGwIndex.end(); ++iter)
	{
		if ((*iter)->GetGwID() == mLocalPeerID)
		{
			(*iter)->AddForce(mGwForceGain);
			break;
		}
	}
}

void GravityWellManager::GwAddRepellF()
{
	std::lock_guard<std::mutex> pt_guard(mustex_gw);
	for (vector<GravityWellClass*>::iterator iter = mGwIndex.begin();
		iter != mGwIndex.end(); ++iter)
	{
		if ((*iter)->GetGwID() == mLocalPeerID)
		{
			(*iter)->AddForce(-mGwForceGain);
			break;
		}
	}
}

void GravityWellManager::ClearForce()
{
	for (vector<GravityWellClass*>::iterator iter = mGwIndex.begin();
		iter != mGwIndex.end(); ++iter)
	{
		if ((*iter)->GetGwID() == mLocalPeerID)
		{
			(*iter)->SetForce(0.f);
			break;
		}
	}
}

std::vector<GravityWellClass*> GravityWellManager::GetGwIndex() const
{
	return mGwIndex;
}

int GravityWellManager::GetNumofGw() const
{
	return mGwIndex.size();
}

void GravityWellManager::GwSetForce(int GwID, float Inforce)
{
	std::lock_guard<std::mutex> pt_guard(mustex_gw);
	for (vector<GravityWellClass*>::iterator iter = mGwIndex.begin();
		iter != mGwIndex.end(); ++iter)
	{
		if ((*iter)->GetGwID() == GwID)
		{
			return (*iter)->SetForce(Inforce);
			break;
		}
	}
}

float GravityWellManager::GwGetForce(int GwID)
{
	std::lock_guard<std::mutex> pt_guard(mustex_gw);
	for (vector<GravityWellClass*>::iterator iter = mGwIndex.begin();
		iter != mGwIndex.end(); ++iter)
	{
		if ((*iter)->GetGwID() == GwID)
		{
			return (*iter)->GetForce();
			break;
		}
	}
}


#include "GravityWellManager.h"



GravityWellManager::GravityWellManager(): 
mGwRadius(2.f)
{
}


GravityWellManager::~GravityWellManager()
{
}

bool GravityWellManager::Initialise(int OwnerID)
{
	ForceGain = 1.f;
	SimpleMath::Color GwColor;
	switch(OwnerID)
	{
		case 0:
		{
			GwColor = Colors::Black;
			break;
		}
		case 1:
		{
			GwColor = Colors::HotPink;
			break;
		}
		case 2:
		{
			GwColor = Colors::Lime;
			break;
		}
	}
	this->AddGw(OwnerID, SimpleMath::Vector3::Zero, GwColor);
	return false;
}

bool GravityWellManager::InitialiseGraphic(shared_ptr<D3DClass> InDirect3D)
{
	assert(mDirect3D = InDirect3D);
	mStates = std::make_unique<DirectX::CommonStates>(mDirect3D->GetDevice());

	mGwEffect = make_unique<DirectX::BasicEffect>(mDirect3D->GetDevice());
	//mGwEffect->SetAlpha(.3f);

	mGwPrimitive = GeometricPrimitive::CreateSphere(mDirect3D->GetDeviceContext(), mGwRadius);
	mGwPrimitive->CreateInputLayout(mGwEffect.get(), mGwInputLayout.ReleaseAndGetAddressOf());
	mGwPrimitive->CreateInputLayout(mGwEffect.get(), mGwInputLayout.ReleaseAndGetAddressOf());

	//Center Indication for Gw
	mGwCenter = GeometricPrimitive::CreateSphere(mDirect3D->GetDeviceContext(), mGwRadius / 10.f);

	return false;
}

void GravityWellManager::Render(SimpleMath::Matrix InView)
{
	SimpleMath::Matrix Proj = SimpleMath::Matrix::Identity;
	mDirect3D->GetProj(Proj);
	mGwEffect->SetProjection(Proj);
	mGwEffect->SetView(InView);
	for (GravityWellClass* element : mGwIndex)
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

void GravityWellManager::AddGw(int InGwID, SimpleMath::Vector3 InGravityWellPos, SimpleMath::Color InGwColor)
{
	GravityWellClass *newGw = new GravityWellClass();
	newGw->Initialize(InGwID, InGravityWellPos, InGwColor);
	mGwIndex.push_back(newGw);
}

void GravityWellManager::RemoveGw(int GwID)
{
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

void GravityWellManager::SetLocalID(int GwID)
{
	mLocalID = GwID;
}

void GravityWellManager::GwSetPos(int GwID, SimpleMath::Vector3 InGravityWellPos)
{
	for (vector<GravityWellClass*>::iterator iter = mGwIndex.begin();
		iter != mGwIndex.end(); ++iter)
	{
		if ((*iter)->GetGwID() == GwID)
		{
			(*iter)->SetPos(InGravityWellPos);
			break;
		}
	}
}

SimpleMath::Vector3 GravityWellManager::GwGetPos(int GwID)
{
	for (vector<GravityWellClass*>::iterator iter = mGwIndex.begin();
		iter != mGwIndex.end(); ++iter)
	{
		if ((*iter)->GetGwID() == GwID)
		{
			return (*iter)->GetPos();			
		}
		else
		{
			return SimpleMath::Vector3::Zero;
		}
	}
}

void GravityWellManager::GwAddMove(int GwID, SimpleMath::Vector3 InMove)
{
	for (vector<GravityWellClass*>::iterator iter = mGwIndex.begin();
		iter != mGwIndex.end(); ++iter)
	{
		if ((*iter)->GetGwID() == GwID)
		{
			(*iter)->AddMove(InMove);
			break;
		}
	}
}

void GravityWellManager::GwAddAttractF(int GwID)
{
	for(vector<GravityWellClass*>::iterator iter = mGwIndex.begin();
		iter!=mGwIndex.end();++iter)
	{
		if((*iter)->GetGwID() == GwID)
		{
			(*iter)->AddForce(ForceGain);
			break;
		}
	}
}

void GravityWellManager::GwAddRepellF(int GwID)
{
	for (vector<GravityWellClass*>::iterator iter = mGwIndex.begin();
		iter != mGwIndex.end(); ++iter)
	{
		if ((*iter)->GetGwID() == GwID)
		{
			(*iter)->AddForce(-ForceGain);
			break;
		}
	}
}

void GravityWellManager::ClearForce(int GwID)
{
	for (vector<GravityWellClass*>::iterator iter = mGwIndex.begin();
		iter != mGwIndex.end(); ++iter)
	{
		if ((*iter)->GetGwID() == GwID)
		{
			(*iter)->SetForce(0.f);
			break;
		}
	}
}

float GravityWellManager::GwGetForce(int GwID)
{
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

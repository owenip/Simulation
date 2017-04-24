#include "GravityWellClass.h"



GravityWellClass::GravityWellClass()
{
	
}


GravityWellClass::~GravityWellClass()
{
}

void GravityWellClass::Initialize(int InID, SimpleMath::Vector3 InPosition, SimpleMath::Color InColor)
{
	mGwID = InID;
	mGravityWellPos = InPosition;
	mGwColor = InColor;
	mForce = 0.f;
}

void GravityWellClass::UpdatePos(SimpleMath::Vector3 InPosition)
{
	mGravityWellPos = InPosition;
}

void GravityWellClass::SetGwIDID(int InID)
{
	mGwID = InID;
}

int GravityWellClass::GetGwID() const
{
	return mGwID;
}

SimpleMath::Color GravityWellClass::GetColor() const
{
	return mGwColor;
}

void GravityWellClass::SetForce(float InForce)
{
	mForce = InForce;
}

float GravityWellClass::GetForce() const
{
	return mForce;
}

void GravityWellClass::AddForce(float InForce)
{
	mForce += InForce;
}

void GravityWellClass::ReduceForce(float InForce)
{
	mForce -= InForce;
}

void GravityWellClass::SetPos(SimpleMath::Vector3 InPos)
{
	mGravityWellPos = InPos;
}

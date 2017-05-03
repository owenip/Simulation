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

SimpleMath::Vector3 GravityWellClass::GetPos() const
{
	return mGravityWellPos;
}

void GravityWellClass::AddMove(SimpleMath::Vector3 move)
{
	mGravityWellPos += move;
}

void GravityWellClass::SetApplyingForce(bool inVal)
{
	mApplyingForce = inVal;
}

bool GravityWellClass::GetApplyingForce() const
{
	return mApplyingForce;
}

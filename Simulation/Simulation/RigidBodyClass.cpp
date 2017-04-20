#include "pch.h"
#include "RigidBodyClass.h"



RigidBodyClass::RigidBodyClass()
{
}


RigidBodyClass::~RigidBodyClass()
{
}

void RigidBodyClass::SetMass(const float & InMass)
{
	fMass = InMass;
}

float RigidBodyClass::GetMass() const
{
	return fMass;
}

void RigidBodyClass::SetInertia(const SimpleMath::Matrix & InMatrix)
{
	mInertia = InMatrix;
}

SimpleMath::Matrix RigidBodyClass::GetInertia() const
{
	return mInertia;
}

void RigidBodyClass::GetInertia(SimpleMath::Matrix & InMatrix) const
{
	InMatrix = mInertia;
}

void RigidBodyClass::SetInertiaInverse(const SimpleMath::Matrix & InMatrix)
{
	mInertiaInverse = InMatrix;
}

SimpleMath::Matrix RigidBodyClass::GetInertiaInverse() const
{
	return mInertiaInverse;
}

void RigidBodyClass::GetInertiaInverse(SimpleMath::Matrix & InMatrix) const
{
	InMatrix = mInertiaInverse;
}

void RigidBodyClass::SetPosition(const SimpleMath::Vector3 & InV)
{
	vPosition = InV;
}

void RigidBodyClass::SetPosition(const float x, const float y, const float z)
{
	vPosition.x = x;
	vPosition.y = y;
	vPosition.z = z;
}

SimpleMath::Vector3 RigidBodyClass::GetPosition() const
{
	return vPosition;
}

void RigidBodyClass::GetPosition(SimpleMath::Vector3 &InV) const
{
	InV = vPosition;
}

void RigidBodyClass::SetVelocity(const SimpleMath::Vector3 & InV)
{
	vVelocity = InV;
}

SimpleMath::Vector3 RigidBodyClass::GetVelocity() const
{
	return vVelocity;
}

void RigidBodyClass::GetVelocity(SimpleMath::Vector3 & InV) const
{
	InV = vVelocity;
}

void RigidBodyClass::SetAcceleration(const SimpleMath::Vector3 & InV)
{
	vAcceleration = InV;
}

SimpleMath::Vector3 RigidBodyClass::GetAcceleration() const
{
	return vAcceleration;
}

void RigidBodyClass::GetAcceleration(SimpleMath::Vector3 & InV) const
{
	InV = vAcceleration;
}


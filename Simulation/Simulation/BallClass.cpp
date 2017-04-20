#include "pch.h"
#include "RigidBodyClass.h"
#include "BallClass.h"



BallClass::BallClass()
{
}


BallClass::~BallClass()
{
}

void BallClass::Initialize(int BallID, int OwenerID, float Radius, float mass, SimpleMath::Vector3 Position, SimpleMath::Vector3 Velocity, SimpleMath::Vector3 Accerleration, float damping)
{
	mBallID = BallID;
	mOwenerID = OwenerID;
	mRadius = Radius;
	
	this->SetMass(mass);
	this->SetPosition(Position);
	this->SetVelocity(Velocity);
	fSpeed = 0.f;
	vVelocityBody = SimpleMath::Vector3::Zero;
	this->SetAcceleration(Accerleration);

	float Ixx, Iyy, Izz;
	Ixx = 2.0f * fMass / 5.0f * (mRadius * mRadius);
	Izz = Iyy = Ixx;
	
	mInertia = SimpleMath::Matrix::Identity;
	mInertia._11 = Ixx;
	mInertia._22 = Iyy;
	mInertia._33 = Izz;
	mInertia.Invert(mInertiaInverse);

	vAngularAcceleration = SimpleMath::Vector3::Zero;
	vAngularAccelerationGlobal = SimpleMath::Vector3::Zero;
	vAngularVelocity = SimpleMath::Vector3::Zero;
	vAngularVelocityGlobal = SimpleMath::Vector3::Zero;
	vEulerAngles = SimpleMath::Vector3::Zero;
	
	qOrientation = SimpleMath::Quaternion::Identity;
	vForces = SimpleMath::Vector3::Zero;
	vMoments = SimpleMath::Vector3::Zero;

}

void BallClass::Update(float dt)
{
	//this->Integrate(dt);
}

void BallClass::Shutdown()
{

}

void BallClass::SetBallID(const int BallID)
{
	mBallID = BallID;
}

int BallClass::GetBallId() const
{
	return mBallID;
}

void BallClass::SetOwenerID(const int owenerID)
{
	mOwenerID = owenerID;
}

int BallClass::GetOwenerID() const
{
	return mOwenerID;
}

void BallClass::SetRadisu(const float radius)
{
	assert(radius != 0);
	mRadius = radius;
}

float BallClass::GetRadius() const
{
	return mRadius;
}

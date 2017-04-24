#include "pch.h"
#include "ParticleClass.h"
#include "BallClass.h"



BallClass::BallClass()
{
}


BallClass::~BallClass()
{
}

void BallClass::Initialize(int BallID, int OwenerID, float Radius, float mass, Vector3 Position, Vector3 Velocity, Vector3 Accerleration, float damping)
{
	mBallID = BallID;
	mOwenerID = OwenerID;
	mRadius = Radius;
	
	this->SetMass(mass);
	this->SetPosition(Position);
	this->SetVelocity(Velocity);
	this->SetAcceleration(Accerleration);
	this->SetDamping(damping);

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

void BallClass::SetRadius(const float radius)
{
	assert(radius != 0);
	mRadius = radius;
}

float BallClass::GetRadius() const
{
	return mRadius;
}

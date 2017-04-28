#include "pch.h"
#include "BallClass.h"

BallClass::BallClass()
{

}

BallClass::BallClass(int BallID, int OwenerID, float Radius, float mass, SimpleMath::Vector3 Position, SimpleMath::Vector3 Velocity, SimpleMath::Vector3 Accerleration, float damping)
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

BallClass::BallClass(int BallID, int OwenerID, float Radius, float mass, SimpleMath::Vector3 Position)
{
	mBallID = BallID;
	mOwenerID = OwenerID;
	mRadius = Radius;
	this->SetMass(mass);
	this->SetPosition(Position);
	mMomentum = SimpleMath::Vector3::Zero;
	mOrientation = SimpleMath::Quaternion::Identity;
	mAngularMomentum = SimpleMath::Vector3::Zero;
	mInertiaTensor = this->GetMass() * mRadius * mRadius * 2 / 3;
	mInverseInertiaTensor = 1.0f / mInertiaTensor;
	this->recalculate();
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
	this->SetAcceleration(Accerleration);
	this->SetDamping(damping);
}



void BallClass::Integrate(float duration)
{
	assert(duration > 0.0);

	mLastPosition = mPosition;
	// Update linear position.
	mPosition += mVelocity * duration;

	// Work out the acceleration from the force.
	SimpleMath::Vector3 resultingAcc = mAcceleration;
	resultingAcc += mForceAccum * mInverseMass;

	//Update linear velocity from the acceleration
	mVelocity += resultingAcc * duration;

	//Impose drag
	mVelocity *= powf(mDamping, duration);
	//mVelocity *= mDamping;

	//Clear the forces
	ClearAccumulator();
}

void BallClass::IntegrateRK(float dt)
{

}



void BallClass::recalculate()
{
	mVelocity = mForceAccum * mInverseMass;
	mAngularVelocity = mAngularMomentum * mInverseInertiaTensor;
	mOrientation.Normalize();
	mSpin = 0.5 * SimpleMath::Quaternion(mAngularVelocity, 0) * mOrientation;
	SimpleMath::Matrix translation;
	translation.Translation(mPosition);
	translation.Transform(translation, mOrientation, bodyToWorld);
	worldToBody = bodyToWorld.Invert();
}



void BallClass::Update(float dt)
{
	//this->Integrate(dt);

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

void BallClass::SetMass(const float mass)
{
	assert(mass != 0);
	mInverseMass = 1.0f / mass;
}

float BallClass::GetMass() const
{
	if (mInverseMass == 0)
	{
		return FLT_MAX;
	}
	else
	{
		return 1.0f / mInverseMass;
	}
}

void BallClass::SetInverseMass(const float inverseMass)
{
	mInverseMass = inverseMass;
}

float BallClass::GetInverseMass() const
{
	return mInverseMass;
}

void BallClass::SetDamping(const float damping)
{
	mDamping = damping;
}

float BallClass::GetDamping() const
{
	return mDamping;
}

void BallClass::SetPosition(SimpleMath::Vector3 & position)
{
	mPosition = position;
}

void BallClass::SetPosition(const float x, const float y, const float z)
{
	mPosition.x = x;
	mPosition.y = y;
	mPosition.z = z;
}

void BallClass::GetPosition(SimpleMath::Vector3 &position)
{
	position = mPosition;
}

SimpleMath::Vector3 BallClass::GetPosition() const
{
	return mPosition;
}

void BallClass::SetVelocity(const SimpleMath::Vector3 &velocity)
{
	mVelocity = velocity;
}


void BallClass::SetVelocity(const float x, const float y, const float z)
{
	mVelocity.x = x;
	mVelocity.y = y;
	mVelocity.z = z;
}

void BallClass::GetVelocity(SimpleMath::Vector3 &velocity) const
{
	velocity = mVelocity;
}

SimpleMath::Vector3 BallClass::GetVelocity() const
{
	return mVelocity;
}

void BallClass::SetAcceleration(const SimpleMath::Vector3 & acceleration)
{
	mAcceleration = acceleration;
}

void BallClass::SetAcceleration(const float x, const float y, const float z)
{
	mAcceleration.x = x;
	mAcceleration.y = y;
	mAcceleration.z = z;
}

void BallClass::GetAccleration(SimpleMath::Vector3 & acceleration)
{
	acceleration = mAcceleration;
}

SimpleMath::Vector3 BallClass::GetAccleration() const
{
	return mAcceleration;
}

void BallClass::ClearAccumulator()
{
	mForceAccum = SimpleMath::Vector3::Zero;
}

void BallClass::AddForce(const SimpleMath::Vector3 & force)
{
	mAcceleration += force;
}

void BallClass::SetForce(const SimpleMath::Vector3 & force)
{
	mForceAccum = force;
}

void BallClass::GetForce(SimpleMath::Vector3 & force)
{
	force = mForceAccum;
}

SimpleMath::Vector3 BallClass::GetForce()
{
	return mForceAccum;
}

void BallClass::SetRotation(SimpleMath::Vector3 &Rotation)
{
	mRotation = Rotation;
}

void BallClass::GetRotation(SimpleMath::Vector3 &Rotation)
{
	Rotation = mRotation;
}

SimpleMath::Vector3 BallClass::GetRotation() const
{
	return mRotation;
}
